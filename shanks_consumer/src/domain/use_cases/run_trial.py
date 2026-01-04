"""
Trial execution logic.
Authors: Shevyrov A.N., Yadrentsev I.M.
"""

import traceback
from typing import Mapping, Any

from src.domain.params import (
    BaseAccelParam,
    BaseSeriesParam,
    NumericLike,
)
from src.domain.trial_result import (
    AccelTrialResult,
    ComputedTrialResult,
    ErrorTrialResult,
    NoErrorTrialResult,
    SeriesTrialResult,
    TrialResult,
)
from src.logger import logged_debug


def trial_results_from_series_error(
    series: BaseSeriesParam,
    accel: BaseAccelParam,
    series_argument: Mapping[str, Any],
    exc: Exception,
) -> list[TrialResult]:
    """Generate trial results from a series error.

    Taken when series parameter generation fails.

    :param series: _series parameter
    :type series: BaseSeriesParam
    :param accel: _accel parameter
    :type accel: BaseAccelParam
    :param series_argument: _series argument mapping
    :type series_argument: Mapping[str, Any]
    :param exc: _exception instance
    :type exc: Exception
    :return: _list of trial results with errors
    :rtype: list[TrialResult]
    """

    return [
        TrialResult(
            SeriesTrialResult(
                id=series.id,
                name=series.series_name,
                lim=None,
                arguments=dict(series_argument),
            ),
            AccelTrialResult(
                name=accel.accel_name,
                m_value=m_value,
                additional_args={
                    key: str(value) for key, value in additional_args.items()
                },
            ),
            computed=[],
            error=ErrorTrialResult(
                str(exc),
                traceback.format_exc(),
                {},
            ),
        )
        for m_value in accel.m_values
        for additional_args in accel.argument_combos
    ]


def trial_results_from_accel_error(
    series: BaseSeriesParam,
    series_lim: NumericLike,
    series_argument: Mapping[str, Any],
    accel: BaseAccelParam,
    additional_args: Mapping[str, Any],
    exc: Exception,
):
    """Generate trial results from an acceleration error.

    Taken when acceleration parameter generation fails.

    :param series: _series parameter
    :type series: BaseSeriesParam
    :param series_lim: _series limit value
    :type series_lim: NumericLike
    :param series_argument: _series argument mapping
    :type series_argument: Mapping[str, Any]
    :param accel: _accel parameter
    :type accel: BaseAccelParam
    :param additional_args: _accel additional arguments
    :type additional_args: Mapping[str, Any]
    :param exc: _exception instance
    :type exc: Exception
    :return: _list of trial results with errors
    :rtype: list[TrialResult]
    """
    return [
        TrialResult(
            SeriesTrialResult(
                id=series.id,
                name=series.series_name,
                lim=series_lim,
                arguments=dict(series_argument),
            ),
            AccelTrialResult(
                name=accel.accel_name,
                m_value=m_value,
                additional_args={
                    key: str(value) for key, value in additional_args.items()
                },
            ),
            computed=[],
            error=ErrorTrialResult(str(exc), traceback.format_exc(), {}),
        )
        for m_value in accel.m_values
    ]


@logged_debug
def execute_trial(
    series_accel: tuple[BaseSeriesParam, BaseAccelParam],
) -> list[TrialResult]:
    """Execute a trial for given series and acceleration parameters.

    The common trial execution logic that iterates over all combinations of
    series arguments and acceleration additional arguments, computes the trial
    results, and handles any errors that may occur during the process.

    :param series_accel: _tuple of series and acceleration parameters
    :type series_accel: tuple[BaseSeriesParam, BaseAccelParam]
    :raises ValueError: _n must be positive
    :raises IndexError: _generated series size is insufficient for n
    :return: _list of trial results
    :rtype: list[TrialResult]
    """
    series, accel = series_accel

    n_values = list(accel.n_values)
    m_values = list(accel.m_values)

    results: list[TrialResult] = []

    for series_argument in series.argument_combos:
        try:
            series_result, series_lim = series.obtain_by_argument(
                series_argument, accel.size_floor
            )
        except Exception as exc:
            results.extend(
                trial_results_from_series_error(series, accel, series_argument, exc)
            )
            continue

        for additional_args in accel.argument_combos:
            try:
                accel_instance = accel.create_instance(additional_args)
            except Exception as exc:
                results.extend(
                    trial_results_from_accel_error(
                        series,
                        series_lim,
                        series_argument,
                        accel,
                        additional_args,
                        exc,
                    )
                )
                continue

            for m_value in m_values:
                computed: list[ComputedTrialResult] = []
                error: ErrorTrialResult | None = None

                ctx = accel.create_event_context()

                for n_value in n_values:
                    if ctx["blocked"]:
                        break
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
                                events=[],
                            )
                        )

                        current_events = accel.process_events(computed, ctx)
                        computed[-1].events = current_events

                    except Exception as exc:
                        error = ErrorTrialResult(
                            str(exc),
                            traceback.format_exc(),
                            {
                                "n": n_value,
                                "m": m_value,
                                "argument": dict(series_argument),
                                "additional_args": {
                                    key: str(value)
                                    for key, value in additional_args.items()
                                },
                            },
                        )
                        break

                results.append(
                    TrialResult(
                        SeriesTrialResult(
                            id=series.id,
                            name=series.series_name,
                            lim=series_lim,
                            arguments=dict(series_argument),
                        ),
                        AccelTrialResult(
                            name=accel.accel_name,
                            m_value=m_value,
                            additional_args={
                                key: str(value)
                                for key, value in additional_args.items()
                            },
                        ),
                        computed=computed,
                        error=error or NoErrorTrialResult,
                    )
                )

    return results
