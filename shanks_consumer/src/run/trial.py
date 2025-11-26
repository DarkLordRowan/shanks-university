import itertools
import logging
import multiprocessing as mp
import uuid
from dataclasses import asdict, dataclass, field
from multiprocessing import Value
from typing import Any, Callable, Iterable, Mapping, TypeGuard

from tqdm import tqdm  # type: ignore[import]

from pyshanks.pyshanks import NumeratorType, RemainderType
from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.precision import (
    SeriesBaseProto,
    SeriesResultProto,
    cast_precision_value,
    cast_real_subtype_value,
)

logger = logging.getLogger(__name__)


def get_next_series_id(counter: Any) -> int:
    """Get next series ID in a thread-safe manner."""
    with counter.get_lock():
        current_id = counter.value
        counter.value += 1
        return current_id


def _is_series_generator(candidate: object) -> TypeGuard[SeriesBaseProto[Any]]:
    return hasattr(candidate, "generateSeries") and hasattr(candidate, "get_sum")


# TODO: replace dataclasses with functions that just constrct dicts. Yep, that stupid.


@dataclass
class SeriesPoint:
    n: int
    value: Any  # partial sum


@dataclass
class AccelPoint:  # zipped with SeriesPoint
    value: Any  # partial sum
    deviation: Any  # unnecessary? Absolutely. But I don't want to get headbonked for losing precision


@dataclass
class SeriesRecord:  # stored in parquet/series
    series_name: str  # partitioned by series name
    series_id: int
    precision: PrecisionType
    source_arguments: dict[str, str]
    series_limit: Any
    computed: list[SeriesPoint]


@dataclass
class ErrorRecord:
    message: str
    n: int | None


@dataclass
class EventRecord:
    name: str
    n: int
    description: str


@dataclass
class AccelRecord:  # stored in parquet/accerations
    series_id: int  # partitioned by series_id
    accel_name: str
    m_value: int
    source_additional_args: dict[str, str]
    computed: list[AccelPoint | None]  # zipped with series.computed
    errors: list[ErrorRecord] | None = None
    events: list[EventRecord] | None = None


def convert_arg_accel(precision, key, value) -> Any:
    if key == "remainder":
        return getattr(RemainderType, str(value))
    elif key == "numerator":
        return getattr(NumeratorType, str(value))
    else:
        return cast_real_subtype_value(precision, value)


def execute_accels(
    precision: PrecisionType,
    series_id: int,
    series_result: SeriesResultProto,
    accels: BaseAccelParam,
    series_limit: Any,
) -> list[AccelRecord]:
    accel_records = []

    for series_argument_list in itertools.product(*accels.additional_args.values()):
        source_arguments = {
            k: v for k, v in zip(accels.additional_args.keys(), series_argument_list)
        }
        arguments = {
            k: convert_arg_accel(precision, k, v) for k, v in source_arguments.items()
        }

        try:
            accel_instance = accels.executable(**arguments)
            for m_value in accels.m_values:
                computed_points: list[AccelPoint | None] = [None] * max(
                    accels.n_values, default=0
                )
                errors = []
                events = []

                prev_accel_deviation = None
                for n_value in accels.n_values:
                    try:
                        if n_value <= 0:
                            raise ValueError("n must be positive")
                        index = n_value - 1
                        if index >= len(series_result.Sn):
                            raise IndexError(
                                f"Generated series size {len(series_result.Sn)} is insufficient for n={n_value}"
                            )

                        # accel_instance returns a SINGLE computed point for this n
                        accel_value = accel_instance(n_value, m_value, series_result)

                        # Check for events (e.g., acceleration is better than partial sum)
                        partial_sum = series_result.Sn[index]

                        accel_deviation = abs(accel_value - series_limit)
                        partial_sum_deviation = abs(partial_sum - series_limit)

                        # Store the computed point with deviation
                        computed_points[n_value - 1] = AccelPoint(
                            value=accel_value,
                            deviation=accel_deviation,
                        )

                        # Event: acceleration deviation is less than partial sum deviation
                        if accel_deviation < partial_sum_deviation:
                            events.append(
                                EventRecord(
                                    name="accel_better_than_partial",
                                    n=n_value,
                                    description=f"accel_value_deviation {accel_deviation} is lesser than partial_sum_deviation {partial_sum_deviation}",
                                )
                            )

                        # Event: acceleration is diverging (deviation increases)
                        if prev_accel_deviation is not None:
                            if prev_accel_deviation < accel_deviation:
                                events.append(
                                    EventRecord(
                                        name="divergent_accel_method",
                                        n=n_value,
                                        description=f"accel_value_deviation from previous iteration {prev_accel_deviation} is lesser than current accel_value_deviation {accel_deviation}",
                                    )
                                )
                        prev_accel_deviation = accel_deviation

                    except Exception as exc:
                        # Record error for this specific n_value
                        errors.append(
                            ErrorRecord(
                                message=str(exc),
                                n=n_value,
                            )
                        )
                        continue

                accel_records.append(
                    AccelRecord(
                        series_id=series_id,
                        accel_name=accels.accel_name,
                        m_value=m_value,
                        source_additional_args={
                            k: str(v) for k, v in source_arguments.items()
                        },
                        computed=computed_points,
                        errors=errors if errors else None,
                        events=events if events else None,
                    )
                )

        except Exception as exc:
            # Create error records for all m_values if accel instantiation fails
            for m_value in accels.m_values:
                accel_records.append(
                    AccelRecord(
                        series_id=series_id,
                        accel_name=accels.accel_name,
                        m_value=m_value,
                        source_additional_args={
                            k: str(v) for k, v in source_arguments.items()
                        },
                        computed=[],
                        errors=[
                            ErrorRecord(
                                message=str(exc),
                                n=None,
                            )
                        ],
                        events=None,
                    )
                )
            continue

    return accel_records


def convert_arg_series(precision, key, value) -> Any:
    if key in ["vecSize", "addKParameter", "m", "b"]:
        return value
    else:
        return cast_precision_value(precision, value)


def _process_combination_worker(
    args: tuple[PrecisionType, BaseSeriesParam, BaseAccelParam],
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
    precision, series, accel = args
    return execute_series_accels(precision, series, [accel], _global_counter)


# Global variable for shared counter in multiprocessing
_global_counter = None


def _init_worker(counter: Any):
    global _global_counter
    _global_counter = counter


def _process_series_worker(
    args: tuple[PrecisionType, BaseSeriesParam, list[BaseAccelParam]],
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
    precision, series, accel_params = args
    return execute_series_accels(precision, series, accel_params, _global_counter)


def execute_series_accels(
    precision: PrecisionType,
    series: BaseSeriesParam,
    related_accel: list[BaseAccelParam],
    counter: Any,
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
    series_records, accel_records = [], []
    for series_argument_list in itertools.product(*series.arguments.values()):
        source_arguments = {
            k: v for k, v in zip(series.arguments.keys(), series_argument_list)
        }
        arguments = {
            k: convert_arg_series(precision, k, v) for k, v in source_arguments.items()
        }

        max_n = max(
            (max(accel.n_values, default=0) for accel in related_accel), default=0
        )
        max_m = max(
            (max(accel.m_values, default=0) for accel in related_accel), default=0
        )
        size_floor = max(10, max_m + max_n + 5)

        try:
            series_candidate = series.executable()
            if not _is_series_generator(series_candidate):
                msg = f"Series executable '{series.series_name}' did not return a valid generator"
                raise TypeError(msg)

            vec_size = int(arguments.get("vecSize", size_floor))
            vec_size = max(vec_size, size_floor)

            default_t = cast_precision_value(precision, 1)
            add_t_value = arguments.get("addTParameter", arguments.get("a", default_t))

            add_k_source = arguments.get(
                "addKParameter", arguments.get("m", arguments.get("b", 1))
            )
            add_k_value = int(add_k_source) if add_k_source is not None else 1

            default_x = cast_precision_value(precision, 0)
            x_value = arguments.get("x", default_x)

            series_result = series_candidate.generateSeries(
                x_value,
                vec_size,
                add_t_value,
                add_k_value,
            )
            series_limit = series_candidate.get_sum()

            series_id = get_next_series_id(counter)

            series_records.append(
                SeriesRecord(
                    series_name=series.series_name,
                    series_id=series_id,
                    precision=precision,
                    source_arguments={k: str(v) for k, v in source_arguments.items()},
                    series_limit=series_limit,
                    computed=[
                        SeriesPoint(n, value)
                        for n, value in zip(
                            range(1, len(series_result.Sn) + 1), series_result.Sn
                        )  # TOO: an? sure?
                    ],
                )
            )
            for accel in related_accel:
                accel_records += execute_accels(
                    precision, series_id, series_result, accel, series_limit
                )

        except Exception as exc:
            # TODO: NO NOTIFICATION OF ERRORS!
            logger.error(f"Couldn't run series {series.series_name}: {exc}")
            # print("Couldn't run:" + str(series))
            # print(exc)

    return series_records, accel_records


@dataclass
class ComplexTrial:
    series_params: list[BaseSeriesParam]
    accel_params: list[BaseAccelParam]
    precision: PrecisionType

    stack_id: str = field(default_factory=lambda: str(uuid.uuid4()))
    chunk_size: int = 1
    process_count: int = 1
    task_timeout: int = 10

    def execute(self) -> Iterable[tuple[list[SeriesRecord], list[AccelRecord]]]:
        if self.process_count > 1:
            # Initialize shared counter for multiprocessing
            counter = mp.Value("i", 0)
            # Use multiprocessing
            with mp.Pool(
                self.process_count, initializer=_init_worker, initargs=(counter,)
            ) as pool:
                for result in tqdm(
                    pool.imap_unordered(
                        _process_series_worker,
                        (
                            (self.precision, series, self.accel_params)
                            for series in self.series_params
                        ),
                    ),
                    total=len(self.series_params),
                    desc="Processing series",
                ):
                    yield result
        else:
            counter = mp.Value("i", 0)  # Local counter for single process
            # Single process with progress bar
            for series in tqdm(self.series_params, desc="Processing series"):
                series_records, accel_records = execute_series_accels(
                    self.precision, series, self.accel_params, counter
                )
                yield series_records, accel_records
