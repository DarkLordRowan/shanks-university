from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
)

import itertools

from dataclasses import dataclass
from typing import Any, Generator, Iterable, Mapping


def cartesian_dicts(
    d: dict[str, Iterable[Any]],
) -> Generator[dict[str, Any], None, None]:
    keys: list[str] = list(d)
    for vals in itertools.product(*(d[k] for k in keys)):
        yield dict(zip(keys, vals))


@dataclass
class ComputedTrialResult:
    n: int
    series_value: float
    partial_sum: float
    partial_sum_deviation: float
    accel_value: float
    accel_value_deviation: float


@dataclass
class ErrorTrialResult:
    description: str
    data: Mapping[str, Any]


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
    error: ErrorTrialResult | None


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
                    error_n_value = n_value
                    accel_value = self.accel.executable(
                        ready_series,
                        *[additional_args[key] for key in additional_args]
                    )(n_value, m_value)

                    partial_sum = ready_series.S_n(n_value)
                    computed.append(
                        ComputedTrialResult(
                            n=n_value,
                            partial_sum=partial_sum,
                            partial_sum_deviation=abs(
                                partial_sum - series_lim
                            ),
                            series_value=ready_series(n_value),
                            accel_value=accel_value,  # type: ignore
                            accel_value_deviation=abs(
                                accel_value - series_lim
                            ),
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
                            key: str(value)
                            for key, value in additional_args.items()
                        },
                    ),
                    computed=computed,
                    error=error,
                )
            )
        return results


@dataclass
class ComplexTrial:
    series_params: list[BaseSeriesParam]
    accel_params: list[BaseAccelParam]

    def execute(self) -> list[TrialResult]:
        results = []
        for series, accel in itertools.product(
            self.series_params, self.accel_params
        ):
            result = Trial(series, accel).execute()
            results += result
        return results
