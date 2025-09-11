from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
)

import itertools

from dataclasses import dataclass
from typing import Any, Generator, Iterable

def cartesian_dicts(d: dict[str, Iterable[Any]]) -> Generator[dict[str, Any], None, None]:
    keys: list[str] = list(d)
    for vals in itertools.product(*(d[k] for k in keys)):
        yield dict(zip(keys, vals))

@dataclass
class TrialResult:
    series_name: str
    series_argument: float
    accel_name: str
    m_value: int
    computed: list[tuple[float, float]]
    error: str | None

@dataclass
class Trial:
    series: BaseSeriesParam
    accel: BaseAccelParam

    def execute(self) -> list[TrialResult]:
        results = []
        for argument, m_value, additional_args in itertools.product(
            self.series.arguments,
            self.accel.m_values,
            cartesian_dicts(self.accel.additional_args)
        ):
            computed = []
            error = None
            try:
                ready_series = self.series.executable(argument)  # type: ignore
                for n_value in self.accel.n_values:
                    computed.append(self.accel.executable(
                        ready_series, **additional_args
                    )(n_value, m_value))
            except Exception as e:  # TODO more debug info
                error = str(e)

            results.append(
                TrialResult(
                    series_name=self.series.series_name,
                    series_argument=argument,
                    accel_name=self.accel.accel_name,
                    m_value=m_value,
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
