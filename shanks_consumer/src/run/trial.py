import itertools
import logging
import multiprocessing as mp
import uuid
from dataclasses import asdict, dataclass, field
from typing import Any, Callable, Mapping, TypeGuard

from tqdm import tqdm  # type: ignore[import]

from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.precision import SeriesBaseProto, cast_precision_value

logger = logging.getLogger(__name__)


def _is_series_generator(candidate: object) -> TypeGuard[SeriesBaseProto[Any]]:
    return hasattr(candidate, "generateSeries") and hasattr(candidate, "get_sum")


@dataclass
class ComputedTrialResult:
    n: int
    series_value: Any
    partial_sum: Any
    partial_sum_deviation: Any
    accel_value: Any
    accel_value_deviation: Any


@dataclass
class ErrorTrialResult:
    description: str | None
    data: Mapping[str, Any]


NoErrorTrialResult = None


@dataclass
class SeriesTrialResult:
    name: str
    lim: Any
    arguments: Mapping[str, Any]


@dataclass
class AccelTrialResult:
    name: str
    m_value: int
    additional_args: Mapping[str, str]


@dataclass
class EventTrialResult:
    event: str
    data: dict


@dataclass
class EventDataTrialResult:
    computed_index: int
    description: str


@dataclass
class TrialResult:
    series: SeriesTrialResult
    accel: AccelTrialResult
    computed: list[ComputedTrialResult]
    error: ErrorTrialResult | None = None
    stack_id: str | None = None
    events: list[EventTrialResult] | None = field(default=None, init=False, repr=False)

    def load_events(self) -> "TrialResult":
        if self.events is None:
            self.events = self._lazy_events()

        return self

    def _lazy_events(self) -> list[EventTrialResult]:
        def _slow_accel_method(result: TrialResult) -> dict | None:
            for i, compute in enumerate(result.computed):
                if compute.accel_value_deviation < compute.partial_sum_deviation:
                    return asdict(
                        EventDataTrialResult(
                            computed_index=i,
                            description=f"accel_value_deviation"
                            f"{compute.accel_value_deviation}\n"
                            " is lesser than partial_sum_deviation"
                            f"{compute.partial_sum_deviation}",
                        )
                    )
            return None

        def _divergent_accel_method(result: TrialResult) -> dict | None:
            for i in range(1, len(result.computed)):
                if (
                    result.computed[i - 1].accel_value_deviation
                    < result.computed[i].accel_value_deviation
                ):
                    return asdict(
                        EventDataTrialResult(
                            computed_index=i,
                            description=f"accel_value_deviation"
                            f"{result.computed[i - 1].accel_value_deviation}\n"
                            f"from a previous iteration {i - 1}\n"
                            f"is lesser than current accel_value_deviation"
                            f"{result.computed[i].accel_value_deviation}",
                        )
                    )
            return None

        _scan_methods: dict[str, Callable] = {
            "slow_accel_method": _slow_accel_method,
            "divergent_accel_method": _divergent_accel_method,
        }

        events = []

        for name, method in _scan_methods.items():
            event_data = method(self)
            if event_data:
                events.append(EventTrialResult(name, event_data))
        return events


def execute_trial(
    series_accel: tuple[BaseSeriesParam, BaseAccelParam],
) -> list[TrialResult]:
    series, accel = series_accel

    n_values = list(accel.n_values)
    m_values = list(accel.m_values)
    if not n_values or not m_values:
        raise ValueError(
            f"Acceleration '{accel.accel_name}' must provide both n and m values."
        )

    series_arg_items = list(series.arguments.items())
    series_arg_keys = [name for name, _ in series_arg_items]
    series_arg_values = [list(values) for _, values in series_arg_items]
    series_argument_combos = (
        list(itertools.product(*series_arg_values)) if series_arg_keys else [()]
    )

    accel_arg_items = list(accel.additional_args.items())
    accel_arg_keys = [name for name, _ in accel_arg_items]
    accel_arg_values = [list(values) for _, values in accel_arg_items]
    accel_argument_combos = (
        list(itertools.product(*accel_arg_values)) if accel_arg_keys else [()]
    )

    size_floor = max(10, max(n_values) + max(m_values) + 5)
    series_precision = getattr(series, "precision", PrecisionType.F64)

    results: list[TrialResult] = []

    for argument_combo in series_argument_combos:
        argument = dict(zip(series_arg_keys, argument_combo))
        try:
            series_candidate = series.executable()
            if not _is_series_generator(series_candidate):
                msg = f"Series executable '{series.series_name}' did not return a valid generator"
                raise TypeError(msg)
            series_instance = series_candidate

            vec_size = int(argument.get("vecSize", size_floor))
            vec_size = max(vec_size, size_floor)

            default_t = cast_precision_value(series_precision, 1)
            add_t_value = argument.get("addTParameter", argument.get("a", default_t))

            add_k_source = argument.get(
                "addKParameter", argument.get("m", argument.get("b", 1))
            )
            add_k_value = int(add_k_source) if add_k_source is not None else 1

            default_x = cast_precision_value(series_precision, 0)
            x_value = argument.get("x", default_x)

            series_result = series_instance.generateSeries(
                x_value,
                vec_size,
                add_t_value,
                add_k_value,
            )
            series_lim = series_instance.get_sum()
        except Exception as exc:
            for accel_combo in accel_argument_combos:
                additional_args = dict(zip(accel_arg_keys, accel_combo))
                additional_args_display = {
                    key: str(value) for key, value in additional_args.items()
                }
                for m_value in m_values:
                    results.append(
                        TrialResult(
                            SeriesTrialResult(
                                name=series.series_name,
                                lim=None,
                                arguments=dict(argument),
                            ),
                            AccelTrialResult(
                                name=accel.accel_name,
                                m_value=m_value,
                                additional_args=additional_args_display,
                            ),
                            computed=[],
                            error=ErrorTrialResult(
                                str(exc),
                                {
                                    "argument": dict(argument),
                                    "additional_args": additional_args_display,
                                    "m": m_value,
                                },
                            ),
                        )
                    )
            continue

        for accel_combo in accel_argument_combos:
            additional_args = dict(zip(accel_arg_keys, accel_combo))
            additional_args_display = {
                key: str(value) for key, value in additional_args.items()
            }
            try:
                accel_instance = accel.executable(*accel_combo)
            except Exception as exc:
                for m_value in m_values:
                    results.append(
                        TrialResult(
                            SeriesTrialResult(
                                name=series.series_name,
                                lim=series_lim,
                                arguments=dict(argument),
                            ),
                            AccelTrialResult(
                                name=accel.accel_name,
                                m_value=m_value,
                                additional_args=additional_args_display,
                            ),
                            computed=[],
                            error=ErrorTrialResult(
                                str(exc),
                                {
                                    "argument": dict(argument),
                                    "additional_args": additional_args_display,
                                    "m": m_value,
                                },
                            ),
                        )
                    )
                continue

            for m_value in m_values:
                computed: list[ComputedTrialResult] = []
                error: ErrorTrialResult | None = None
                for n_value in n_values:
                    try:
                        if n_value <= 0:
                            raise ValueError("n must be positive")
                        index = n_value - 1
                        if index >= len(series_result.Sn):
                            raise IndexError(
                                f"Generated series size {len(series_result.Sn)} is insufficient for n={n_value}"
                            )
                        partial_sum = series_result.Sn[index]
                        series_term = series_result.an[index]
                        accel_value = accel_instance(n_value, m_value, series_result)
                        computed.append(
                            ComputedTrialResult(
                                n=n_value,
                                series_value=series_term,
                                partial_sum=partial_sum,
                                partial_sum_deviation=abs(partial_sum - series_lim),
                                accel_value=accel_value,
                                accel_value_deviation=abs(accel_value - series_lim),
                            )
                        )
                    except Exception as exc:
                        error = ErrorTrialResult(
                            str(exc),
                            {
                                "n": n_value,
                                "m": m_value,
                                "argument": dict(argument),
                                "additional_args": additional_args_display,
                            },
                        )
                        break

                results.append(
                    TrialResult(
                        SeriesTrialResult(
                            name=series.series_name,
                            lim=series_lim,
                            arguments=dict(argument),
                        ),
                        AccelTrialResult(
                            name=accel.accel_name,
                            m_value=m_value,
                            additional_args=additional_args_display,
                        ),
                        computed=computed,
                        error=error or NoErrorTrialResult,
                    )
                )

    return results


@dataclass
class ComplexTrial:
    series_params: list[BaseSeriesParam]
    accel_params: list[BaseAccelParam]

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

    def _execute_sequential(self) -> list[TrialResult]:
        return list(
            itertools.chain.from_iterable(
                [
                    execute_trial(series_accel)
                    for series_accel in self._trial_combinations
                ]
            )
        )

    def _execute_parallel(self) -> list[TrialResult]:
        if not self._trial_combinations:
            return []

        num_processes = self.process_count or min(
            mp.cpu_count(), len(self._trial_combinations)
        )

        try:
            with mp.Pool(processes=num_processes) as pool:
                results = []

                pending_tasks = [
                    (
                        pool.apply_async(execute_trial, (combination,)),
                        combination,
                    )
                    for combination in self._trial_combinations
                ]

                with tqdm(
                    total=len(self._trial_combinations),
                    desc="Running trials",
                    unit="trial",
                    ncols=100,
                ) as pbar:
                    for async_result, combination in pending_tasks:
                        try:
                            trial_results = async_result.get(timeout=self.task_timeout)
                            if trial_results:
                                results.extend(trial_results)
                        except mp.TimeoutError:
                            series, accel = combination
                            results.append(
                                TrialResult(
                                    SeriesTrialResult(
                                        name=series.series_name,
                                        lim=None,
                                        arguments={},
                                    ),
                                    AccelTrialResult(
                                        name=accel.accel_name,
                                        m_value=-1,
                                        additional_args={},
                                    ),
                                    computed=[],
                                    error=ErrorTrialResult(
                                        "Trial execution failed:"
                                        " execution time exceeded "
                                        f"{self.task_timeout} seconds",
                                        data={
                                            "series": series,
                                            "accel": accel,
                                        },
                                    ),
                                )
                            )
                        finally:
                            pbar.update(1)
                return results
        except Exception:
            # ! emergency fallback
            return self._execute_sequential()

    def execute(self) -> list[TrialResult]:
        results = self._execute_parallel()

        for result in results:
            result.stack_id = self.stack_id

        return results
