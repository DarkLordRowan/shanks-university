from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
)

import itertools

from dataclasses import dataclass
from typing import Any, Generator, Iterable, Mapping

def cartesian_dicts(d: dict[str, Iterable[Any]]) -> Generator[dict[str, Any], None, None]:
    keys: list[str] = list(d)
    for vals in itertools.product(*(d[k] for k in keys)):
        yield dict(zip(keys, vals))


@dataclass
class ComputedTrialResult:
    n: int
    s_n: float
    value: float


@dataclass
class ErrorTrialResult:
    description: str
    data: Mapping[str, Any]


@dataclass
class SeriesTrialResult:
    name: str
    sum: Any
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
            computed, series_sum = [], None
            error, error_n_value = None, None
            try:
                ready_series = self.series.executable(*[argument[key] for key in argument])  # type: ignore
                series_sum = ready_series.get_sum()
                for n_value in self.accel.n_values:
                    error_n_value = n_value
                    computed.append(
                        ComputedTrialResult(
                            n_value,
                            ready_series.S_n(n_value),
                            self.accel.executable(
                                ready_series,
                                *[
                                    additional_args[key]
                                    for key in additional_args
                                ]
                            )(n_value, m_value),
                        )
                    )
            except Exception as e:  # TODO more debug info
                error = ErrorTrialResult(str(e), {"n": error_n_value})

            results.append(
                TrialResult(
                    SeriesTrialResult(
                        name=self.series.series_name,
                        sum=series_sum,
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
            results += Trial(series, accel).execute()
        return results
