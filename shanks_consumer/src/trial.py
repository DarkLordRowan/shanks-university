from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
)

import itertools

from dataclasses import dataclass


@dataclass
class TrialResult:
    series_name: str
    series_argument: float
    accel_name: str
    n_value: int
    m_value: int
    result: float | None
    error: str | None


class SimpleTrial:
    def __init__(self, series: BaseSeriesParam, accel: BaseAccelParam):
        self.series = series
        self.accel = accel

        self.results: list[TrialResult] = []

    def execute(self):
        if not (
            self.series.arguments
            and self.accel.n_values
            and self.accel.m_values
        ):
            raise ValueError
        for argument, n_value, m_value in itertools.product(
            self.series.arguments,
            self.accel.n_values,
            self.accel.m_values,
        ):
            ready_series = self.series.executable(argument)  # type: ignore
            error, result = None, None
            try:
                result = self.accel.executable(
                    ready_series, **self.accel.additional_args
                )(n_value, m_value)
            except Exception as e:  # TODO more debug info
                error = str(e)

            self.results.append(
                TrialResult(
                    series_name=self.series.series_name,
                    series_argument=argument,
                    accel_name=self.accel.accel_name,
                    n_value=n_value,
                    m_value=m_value,
                    result=result,
                    error=error,
                )
            )


class ComplexTrial:

    def __init__(
        self,
        series_params: list[BaseSeriesParam],
        accel_params: list[BaseAccelParam],
    ):
        self.series_params = series_params
        self.accel_params = accel_params
        self.results: list = []

    def execute(self):
        for series, accel in itertools.product(
            self.series_params, self.accel_params
        ):
            st = SimpleTrial(series, accel)
            st.execute()
            self.results += st.results
