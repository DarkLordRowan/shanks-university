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
    value: float


@dataclass
class ErrorTrialResult:
    description: str
    data: Mapping[str, str]


@dataclass
class TrialResult:
    series_name: str
    series_arguments: Mapping[str, Any]
    accel_name: str
    accel_m_value: int
    accel_additional_args: Mapping[str, str]
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
            computed, error, error_n_value = [], None, None
            try:
                ready_series = self.series.executable(*[argument[key] for key in argument])  # type: ignore
                for n_value in self.accel.n_values:
                    error_n_value = n_value
                    computed.append(
                        ComputedTrialResult(
                            n_value,
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
                error = ErrorTrialResult(str(e), {"n": str(error_n_value)})

            results.append(
                TrialResult(
                    series_name=self.series.series_name,
                    series_arguments=argument,
                    accel_name=self.accel.accel_name,
                    accel_m_value=m_value,
                    accel_additional_args={
                        key: str(value)
                        for key, value in additional_args.items()
                    },
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
