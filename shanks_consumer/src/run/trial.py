import itertools
import logging
import multiprocessing as mp
import uuid
from dataclasses import asdict, dataclass, field
from typing import Any, Callable, Mapping, TypeGuard

from tqdm import tqdm  # type: ignore[import]

from pyshanks.pyshanks import NumeratorType, RemainderType
from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.precision import SeriesBaseProto, SeriesResultProto, cast_precision_value

logger = logging.getLogger(__name__)


def _is_series_generator(candidate: object) -> TypeGuard[SeriesBaseProto[Any]]:
    return hasattr(candidate, "generateSeries") and hasattr(candidate, "get_sum")


@dataclass
class SeriesPoint:
    n: int
    value: Any


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
    data: dict[str, Any]


@dataclass
class EventRecord:
    name: str
    data: dict[str, Any]


@dataclass
class AccelRecord:  # stored in parquet/accerations
    series_id: int  # partitioned by series_id
    accel_name: str
    m_value: int
    source_additional_args: dict
    computed: list[Any]  # zipped with series.computed
    errors: list[ErrorRecord] | None = None
    events: list[EventRecord] | None = None


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
        arguments = {k: convert_arg(k, v) for k, v in source_arguments.items()}

        try:
            accel_instance = accels.executable(**arguments)
            for m_value in accels.m_values:
                computed_values = [None] * max(accels.n_values, default=0)
                errors = []
                events = []

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
                        computed_values[n_value - 1] = accel_value

                        # Check for events (e.g., acceleration is better than partial sum)
                        if index < len(series_result.Sn):
                            partial_sum = series_result.Sn[index]

                            if series_limit is not None:
                                accel_deviation = abs(accel_value - series_limit)
                                partial_sum_deviation = abs(partial_sum - series_limit)

                                # Event: acceleration deviation is less than partial sum deviation
                                if accel_deviation < partial_sum_deviation:
                                    events.append(
                                        EventRecord(
                                            name="accel_better_than_partial",
                                            data={
                                                "n": n_value,
                                                "description": f"accel_value_deviation {accel_deviation} is lesser than partial_sum_deviation {partial_sum_deviation}",
                                            },
                                        )
                                    )

                                # Event: acceleration is diverging (deviation increases)
                                if len(computed_values) > 1:
                                    prev_accel_value = computed_values[-2]
                                    prev_deviation = abs(
                                        prev_accel_value - series_limit
                                    )
                                    if prev_deviation < accel_deviation:
                                        events.append(
                                            EventRecord(
                                                name="divergent_accel_method",
                                                data={
                                                    "n": n_value,
                                                    "description": f"accel_value_deviation from previous iteration {prev_deviation} is lesser than current accel_value_deviation {accel_deviation}",
                                                },
                                            )
                                        )

                    except Exception as exc:
                        # Record error for this specific n_value
                        errors.append(
                            ErrorRecord(
                                message=str(exc),
                                data={
                                    "n": n_value,
                                    "m": m_value,
                                    "arguments": source_arguments,
                                },
                            )
                        )
                        continue

                accel_records.append(
                    AccelRecord(
                        series_id=series_id,
                        accel_name=accels.accel_name,
                        m_value=m_value,
                        source_additional_args=source_arguments,
                        computed=computed_values,
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
                        source_additional_args=source_arguments,
                        computed=[],
                        errors=[
                            ErrorRecord(
                                message=str(exc),
                                data={
                                    "m": m_value,
                                    "arguments": source_arguments,
                                },
                            )
                        ],
                        events=None,
                    )
                )
            continue

    return accel_records


def convert_arg(key, value):
    if key == "remainder":
        return getattr(RemainderType, str(value))
    elif key == "numerator":
        return getattr(NumeratorType, str(value))
    else:
        return value


def _process_combination_worker(
    args: tuple[PrecisionType, BaseSeriesParam, BaseAccelParam],
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
    precision, series, accel = args
    return execute_series_accels(precision, series, [accel])


_fresh = 0


def execute_series_accels(
    precision: PrecisionType,
    series: BaseSeriesParam,
    related_accel: list[BaseAccelParam],
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
    global _fresh
    series_records, accel_records = [], []
    for series_argument_list in itertools.product(*series.arguments.values()):
        source_arguments = {
            k: v for k, v in zip(series.arguments.keys(), series_argument_list)
        }
        arguments = {k: convert_arg(k, v) for k, v in source_arguments.items()}

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

            series_id = _fresh
            _fresh += 1

            series_records.append(
                SeriesRecord(
                    series_name=series.series_name,
                    series_id=series_id,
                    precision=precision,
                    source_arguments=source_arguments,
                    series_limit=series_limit,
                    computed=[
                        SeriesPoint(n, value)
                        for n, value in zip(
                            range(1, len(series_result.an) + 1), series_result.an
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
            pass
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

    _trial_combinations: list[tuple[BaseSeriesParam, BaseAccelParam]] = field(
        init=False
    )

    def __post_init__(self):
        self._trial_combinations = list(
            itertools.product(self.series_params, self.accel_params)
        )

    def execute(self) -> tuple[list[SeriesRecord], list[AccelRecord]]:
        all_series_records = []
        all_accel_records = []

        if self.process_count > 1:
            # Use multiprocessing
            with mp.Pool(self.process_count) as pool:
                results = list(
                    tqdm(
                        pool.imap_unordered(
                            _process_combination_worker,
                            [
                                (self.precision, series, accel)
                                for series, accel in self._trial_combinations
                            ],
                        ),
                        total=len(self._trial_combinations),
                        desc="Processing combinations",
                    )
                )

                for series_records, accel_records in results:
                    all_series_records.extend(series_records)
                    all_accel_records.extend(accel_records)
        else:
            # Single process with progress bar
            for series, accel in tqdm(
                self._trial_combinations, desc="Processing combinations"
            ):
                series_records, accel_records = execute_series_accels(
                    self.precision, series, [accel]
                )
                all_series_records.extend(series_records)
                all_accel_records.extend(accel_records)

        return all_series_records, all_accel_records
