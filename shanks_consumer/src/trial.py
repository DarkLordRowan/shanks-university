from src.params import (
    BaseAccelParam,
    BaseSeriesParam,
)

import itertools

from dataclasses import dataclass
from typing import Any, Generator, Iterable, Mapping
from pyshanks import Arb


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
    arb: bool = True

    def execute(self) -> list[TrialResult]:
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

                # Check if this is a CSV series (has pre-computed data)
                if hasattr(series_instance, 'data'):
                    # CSV series - use pre-computed data directly
                    series_result = series_instance.data
                    series_lim = series_instance.get_sum()
                else:
                    # Regular series - generate using parameters
                    # Extract arguments for generateSeries and convert types based on Arb flag
                    x_arg = argument.get('x', 0.0)
                    vec_size_arg = argument.get('vecSize', 100)
                    add_t_param = argument.get('addTParameter', 1.0)
                    add_k_param = argument.get('addKParameter', 1)

                    # Convert numeric parameters to appropriate type (Arb or float)
                    if self.arb:
                        if not isinstance(x_arg, Arb):
                            x_arg = Arb(str(x_arg))
                        if not isinstance(add_t_param, Arb):
                            add_t_param = Arb(str(add_t_param))
                    else:
                        if isinstance(x_arg, Arb):
                            x_arg = float(x_arg)
                        if isinstance(add_t_param, Arb):
                            add_t_param = float(add_t_param)

                    # Generate the series using the new API
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
                        n_value, m_value, series_result, 0
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
class ComplexTrial:
    series_params: list[BaseSeriesParam]
    accel_params: list[BaseAccelParam]
    arb: bool = True

    def execute(self) -> list[TrialResult]:
        results = []
        for series, accel in itertools.product(
            self.series_params, self.accel_params
        ):
            result = Trial(series, accel, arb=self.arb).execute()
            results += result
        return results
