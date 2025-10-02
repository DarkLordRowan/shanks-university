import itertools
import multiprocessing as mp
import uuid
from dataclasses import dataclass, field
from typing import Any, Generator, Iterable, Mapping

from pyshanks import Arb
from src.params import BaseAccelParam, BaseSeriesParam


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
class TrialResult:
    series: SeriesTrialResult
    accel: AccelTrialResult
    computed: list[ComputedTrialResult]
    error: ErrorTrialResult
    id: str = field(default_factory=lambda: str(uuid.uuid4()))


@dataclass
class Trial:
    series: BaseSeriesParam
    accel: BaseAccelParam

    def execute(self) -> list[TrialResult]:
        results = []
        for argument, m_value, additional_args in itertools.product(
            [
                dict(zip(self.series.arguments.keys(), values))
                for values in zip(*self.series.arguments.values())
            ],
            self.accel.m_values,
            cartesian_dicts(self.accel.additional_args),
        ):
            computed, series_lim = [], None
            error, error_n_value = None, None
            try:
                ready_series = self.series.executable(
                    *[argument[key] for key in argument]
                )
                series_lim = ready_series.get_sum()
                for n_value in self.accel.n_values:
                    # ? stub strange signature
                    error_n_value = n_value
                    accel_value = self.accel.executable(
                        ready_series,  # type: ignore
                        *[additional_args[key] for key in additional_args]
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
                        name=self.series.series_name,
                        lim=series_lim,
                        arguments=argument,
                    ),
                    AccelTrialResult(
                        name=self.accel.accel_name,
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

    chunk_size: int = 1
    process_count: int | None = None

    _trial_combinations: list[tuple[BaseSeriesParam, BaseAccelParam]] = field(
        init=False
    )

    def __post_init__(self):
        self._trial_combinations = list(
            itertools.product(self.series_params, self.accel_params)
        )

    @staticmethod
    def _run_trial(series_accel: tuple[BaseSeriesParam, BaseAccelParam]):
        return Trial(*series_accel).execute()

    def _execute_sequential(self) -> list[TrialResult]:
        results = []
        for series, accel in self._trial_combinations:
            result = Trial(series, accel).execute()
            results += result
        return results

    def _execute_parallel(self) -> list[TrialResult]:
        if not self._trial_combinations:
            return []

        num_processes = self.process_count or min(
            mp.cpu_count(), len(self._trial_combinations)
        )

        if num_processes == 1:
            return self._execute_sequential()

        try:
            with mp.Pool(processes=num_processes) as pool:
                chunked_results = pool.imap_unordered(
                    ComplexTrial._run_trial,
                    self._trial_combinations,
                    chunksize=self.chunk_size,
                )
                results = []
                for trial_results in chunked_results:
                    results.extend(trial_results)

                return results
        except Exception:
            # TODO log it pls
            return self._execute_sequential()

    def execute(self) -> list[TrialResult]:
        if self.process_count:
            return self._execute_parallel()
        return self._execute_sequential()
