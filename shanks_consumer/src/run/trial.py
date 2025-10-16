import itertools
import logging
import multiprocessing as mp
import uuid
from dataclasses import asdict, dataclass, field
from typing import Any, Callable, Generator, Iterable, Mapping

from pyshanks import Arb
from src.run.params import BaseAccelParam, BaseSeriesParam
from tqdm import tqdm

logger = logging.getLogger(__name__)


def cartesian_dicts(
    d: dict[str, Iterable[Any]],
) -> Generator[dict[str, Any], None, None]:
    keys: list[str] = list(d)
    for vals in itertools.product(*(d[k] for k in keys)):
        yield dict(zip(keys, vals))


@dataclass
class ComputedTrialResult:
    n: int
    series_value: float | Arb
    partial_sum: float | Arb
    partial_sum_deviation: float | Arb
    accel_value: float | Arb
    accel_value_deviation: float | Arb


@dataclass
class ErrorTrialResult:
    description: str | None
    data: Mapping[str, Any]


NoErrorTrialResult = ErrorTrialResult(None, {})


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
    error: ErrorTrialResult
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
    results = []
    series, accel = series_accel

    for argument, m_value, additional_args in itertools.product(
        [
            dict(zip(series.arguments.keys(), values))
            for values in zip(*series.arguments.values())
        ],
        accel.m_values,
        cartesian_dicts(accel.additional_args),
    ):
        computed, series_lim = [], None
        error, error_n_value = None, None
        try:
            ready_series = series.executable(*[argument[key] for key in argument])
            series_lim = ready_series.get_sum()
            for n_value in accel.n_values:
                # ? stub strange signature
                error_n_value = n_value
                accel_value = accel.executable(
                    ready_series,  # type: ignore
                    *[additional_args[key] for key in additional_args],
                )(n_value, m_value)

                partial_sum = ready_series.S_n(n_value)
                computed.append(
                    # ? for now, we assume partial_sum, accel_value,
                    # ? and series_lim are all of the same type, either float or Arb
                    ComputedTrialResult(
                        n=n_value,
                        partial_sum=partial_sum,
                        partial_sum_deviation=abs(
                            partial_sum - series_lim
                        ),  # type: ignore
                        series_value=ready_series(n_value),
                        accel_value=accel_value,
                        accel_value_deviation=abs(
                            accel_value - series_lim
                        ),  # type: ignore
                    )
                )
        except Exception as e:  # TODO more debug info
            error = ErrorTrialResult(str(e), {"n": error_n_value})

        results.append(
            TrialResult(
                SeriesTrialResult(
                    name=series.series_name,
                    lim=series_lim,
                    arguments=argument,
                ),
                AccelTrialResult(
                    name=accel.accel_name,
                    m_value=m_value,
                    additional_args={
                        key: str(value) for key, value in additional_args.items()
                    },
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
