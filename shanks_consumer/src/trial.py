from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
    NumericLike,
    SeriesParamCSV,
    SeriesResultProto,
    TNum,
    PrecisionType,
)
import pyshanks as ps

import itertools

from dataclasses import dataclass
from typing import Any, Generator, Iterable, Mapping, Generic


def cartesian_dicts(
    d: dict[str, Iterable[Any]],
) -> Generator[dict[str, Any], None, None]:
    keys: list[str] = list(d)
    for vals in itertools.product(*(d[k] for k in keys)):
        yield dict(zip(keys, vals))


@dataclass
class ComputedTrialResult(Generic[TNum]):
    n: int
    series_value: TNum
    partial_sum: TNum
    partial_sum_deviation: TNum
    accel_value: TNum
    accel_value_deviation: TNum


@dataclass
class ErrorTrialResult:
    description: str
    data: Mapping[str, Any]


@dataclass
class SeriesTrialResult(Generic[TNum]):
    name: str
    lim: TNum
    arguments: Mapping[str, Any]


@dataclass
class AccelTrialResult:
    name: str
    m_value: int
    additional_args: Mapping[str, str]


@dataclass
class TrialResult(Generic[TNum]):
    series: SeriesTrialResult[TNum]
    accel: AccelTrialResult
    computed: list[ComputedTrialResult[TNum]]
    error: ErrorTrialResult | None


@dataclass
class Trial(Generic[TNum]):
    series: BaseSeriesParam[TNum]
    accel: BaseAccelParam[TNum]
    precision: PrecisionType = PrecisionType.F64

    def execute(self) -> list[TrialResult[TNum]]:
        results = []
        # Handle series with no arguments (like CSV series)
        arguments_list = [
            dict(zip(self.series.arguments.keys(), values))
            for values in zip(*self.series.arguments.values())
        ] if self.series.arguments else [{}]

        for argument, m_value, additional_args in itertools.product(
            arguments_list,
            self.accel.m_values,
            cartesian_dicts(self.accel.additional_args),
        ):
            computed, series_lim = [], None
            error, error_n_value = None, None
            try:
                # For series that need generateSeries method
                series_instance = self.series.executable()

                if isinstance(series_instance, SeriesParamCSV):
                    # CSV series - use pre-computed data directly
                    series_result = series_instance.data
                    series_lim = series_instance.get_sum()
                else:
                    # Regular series - generate using parameters
                    # Extract arguments for generateSeries and convert types based on precision
                    x_arg = argument.get('x', 0.0)
                    vec_size_arg = argument.get('vecSize', 100)
                    add_t_param = argument.get('addTParameter', 1.0)
                    add_k_param = argument.get('addKParameter', 1)

                    series_result = series_instance.generateSeries(
                        x_arg, vec_size_arg, add_t_param, add_k_param
                    )
                    series_lim = series_instance.get_sum()


                for n_value in self.accel.n_values:
                    error_n_value = n_value
                    # Create acceleration method instance with additional args
                    accel_instance = self.accel.executable(
                        *[additional_args[key] for key in additional_args]
                    )
                    # Call the acceleration method using operator()
                    accel_value = accel_instance(
                        n_value, m_value, series_result
                    )

                    partial_sum = series_result.Sn[n_value] if n_value < len(series_result.Sn) else series_result.Sn[-1]
                    series_value = series_result.an[n_value] if n_value < len(series_result.an) else series_result.an[-1]
                    computed.append(
                        ComputedTrialResult(
                            n=n_value,
                            partial_sum=partial_sum,
                            partial_sum_deviation=abs(
                                partial_sum - series_lim
                            ),
                            series_value=series_value,
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
class ComplexTrial(Generic[TNum]):
    series_params: list[BaseSeriesParam[TNum]]
    accel_params: list[BaseAccelParam[TNum]]
    precision: PrecisionType = PrecisionType.F64

    def execute(self) -> list[TrialResult[TNum]]:
        results = []
        for series, accel in itertools.product(
            self.series_params, self.accel_params
        ):
            result = Trial(series, accel, precision=self.precision).execute()
            results += result
        return results
