import itertools
from functools import lru_cache
from typing import Any, TypeGuard

from src.domain.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.domain.precision import SeriesBaseProto, cast_precision_value
from src.domain.trial_result import (
    AccelTrialResult,
    ComputedTrialResult,
    ErrorTrialResult,
    NoErrorTrialResult,
    SeriesTrialResult,
    TrialResult,
)

from src.domain.event import EVENT_METHODS


@lru_cache(maxsize=256)
def cached_generate_series(
    series_type: type, x: Any, vec_size: int, t: Any, k: int
):
    series = series_type()
    return (
        series.generateSeries(x, vec_size, t, k),
        series.get_sum(),
    )


def _is_series_generator(candidate: object) -> TypeGuard[SeriesBaseProto[Any]]:
    return hasattr(candidate, "generateSeries") and hasattr(candidate, "get_sum")


def execute_trial(
    series_accel: tuple[BaseSeriesParam, BaseAccelParam],
) -> list[TrialResult]:
    series, accel = series_accel

    n_values = list(accel.n_values)
    m_values = list(accel.m_values)
    if not n_values or not m_values:
        raise ValueError(
            f"Acceleration '{accel.accel_name}' must provide both n and m values."
        )

    series_arg_items = list(series.arguments.items())
    series_arg_keys = [name for name, _ in series_arg_items]
    series_arg_values = [list(values) for _, values in series_arg_items]
    series_argument_combos = (
        list(itertools.product(*series_arg_values)) if series_arg_keys else [()]
    )

    accel_arg_items = list(accel.additional_args.items())
    accel_arg_keys = [name for name, _ in accel_arg_items]
    accel_arg_values = [list(values) for _, values in accel_arg_items]
    accel_argument_combos = (
        list(itertools.product(*accel_arg_values)) if accel_arg_keys else [()]
    )

    size_floor = max(10, max(n_values) + max(m_values) + 5)
    series_precision = getattr(series, "precision", PrecisionType.F64)

    results: list[TrialResult] = []

    for argument_combo in series_argument_combos:
        argument = dict(zip(series_arg_keys, argument_combo))
        try:
            series_candidate = series.executable()
            if not _is_series_generator(series_candidate):
                msg = f"Series executable '{series.series_name}' did not return a valid generator"
                raise TypeError(msg)

            vec_size = int(argument.get("vecSize", size_floor))
            vec_size = max(vec_size, size_floor)

            default_t = cast_precision_value(series_precision, 1)
            add_t_value = argument.get("addTParameter", argument.get("a", default_t))

            add_k_source = argument.get(
                "addKParameter", argument.get("m", argument.get("b", 1))
            )
            add_k_value = int(add_k_source) if add_k_source is not None else 1

            default_x = cast_precision_value(series_precision, 0)
            x_value = argument.get("x", default_x)

            series_result, series_lim = cached_generate_series(
                type(series_candidate),
                x_value,  # type: ignore
                vec_size,
                add_t_value,  # type: ignore
                add_k_value,
            )
        except Exception as exc:
            for accel_combo in accel_argument_combos:
                additional_args = dict(zip(accel_arg_keys, accel_combo))
                additional_args_display = {
                    key: str(value) for key, value in additional_args.items()
                }
                for m_value in m_values:
                    results.append(
                        TrialResult(
                            SeriesTrialResult(
                                name=series.series_name,
                                lim=None,
                                arguments=dict(argument),
                            ),
                            AccelTrialResult(
                                name=accel.accel_name,
                                m_value=m_value,
                                additional_args=additional_args_display,
                            ),
                            computed=[],
                            error=ErrorTrialResult(
                                str(exc),
                                {
                                    "argument": dict(argument),
                                    "additional_args": additional_args_display,
                                    "m": m_value,
                                },
                            ),
                        )
                    )
            continue

        for accel_combo in accel_argument_combos:
            additional_args = dict(zip(accel_arg_keys, accel_combo))
            additional_args_display = {
                key: str(value) for key, value in additional_args.items()
            }
            try:
                accel_instance = accel.executable(*accel_combo)
            except Exception as exc:
                for m_value in m_values:
                    results.append(
                        TrialResult(
                            SeriesTrialResult(
                                name=series.series_name,
                                lim=series_lim,
                                arguments=dict(argument),
                            ),
                            AccelTrialResult(
                                name=accel.accel_name,
                                m_value=m_value,
                                additional_args=additional_args_display,
                            ),
                            computed=[],
                            error=ErrorTrialResult(
                                str(exc),
                                {
                                    "argument": dict(argument),
                                    "additional_args": additional_args_display,
                                    "m": m_value,
                                },
                            ),
                        )
                    )
                continue

            for m_value in m_values:
                computed: list[ComputedTrialResult] = []
                error: ErrorTrialResult | None = None
                for n_value in n_values:
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

                        events = list(
                            filter(
                                None,
                                [
                                    EVENT_METHODS.get(
                                        event.type, lambda _: None
                                    )(computed)
                                    for event in accel.events
                                ],
                            )
                        )
                        computed.append(
                            ComputedTrialResult(
                                n=n_value,
                                series_value=series_term,
                                partial_sum=partial_sum,
                                partial_sum_deviation=abs(
                                    partial_sum - series_lim
                                ),
                                accel_value=accel_value,
                                accel_value_deviation=abs(
                                    accel_value - series_lim
                                ),
                                events=events,  # type: ignore
                            )
                        )
                    except Exception as exc:
                        error = ErrorTrialResult(
                            str(exc),
                            {
                                "n": n_value,
                                "m": m_value,
                                "argument": dict(argument),
                                "additional_args": additional_args_display,
                            },
                        )
                        break

                results.append(
                    TrialResult(
                        SeriesTrialResult(
                            name=series.series_name,
                            lim=series_lim,
                            arguments=dict(argument),
                        ),
                        AccelTrialResult(
                            name=accel.accel_name,
                            m_value=m_value,
                            additional_args=additional_args_display,
                        ),
                        computed=computed,
                        error=error or NoErrorTrialResult,
                    )
                )

    return results
