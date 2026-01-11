"""
Event definitions and methods for detecting specific behaviors in trial results.
Author: Shevyrov A.N., Yadrentsev I. M.
"""

from enum import Enum

from src.domain.trial_result import ComputedTrialResult, EventData


class EventType(Enum):
    """Enumeration of event types detected in trial results."""

    SLOW_ACCEL = "slow_accel"
    """Method is slower/worse than partial sums.
    """
    MONOTONE_ACCEL = "monotone"
    """Method is monotone (error magnitude unchanged).
    """
    DIVERGENT_ACCEL = "divergent_accel"
    """Method is divergent (error magnitude growing).
    """
    SIGN_CHANGED = "sign_changed"
    """Error sign is changed (oscillation).
    """
    SECOND_DIFF = "second_diff"
    """|Aₖ − Aₖ₋₁| ≥ |Aₖ₋₁ − Aₖ₋₂|
    """


def slow_accel_method(computed: list[ComputedTrialResult]) -> EventData | None:
    """Detects if the acceleration method is performing worse than the baseline partial sums.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 1:
        return None
    last = computed[-1]
    
    # If acceleration error is GREATER than partial sum error, it's performing poorly.
    if last.accel_value_deviation > last.partial_sum_deviation:
        return EventData(
            name="slow_accel",
            description=(
                f"The acceleration deviation {last.accel_value_deviation} "
                f"is greater than the partial-sum deviation {last.partial_sum_deviation}, "
                "indicating worse accuracy than the baseline sequence."
            ),
        )
    return None


def divergent_accel_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Detects if the acceleration method is diverging (error growing).

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return None
    last, previous = computed[-1], computed[-2]
    
    # If error magnitude is increasing
    if previous.accel_value_deviation < last.accel_value_deviation:
        return EventData(
            name="divergent_accel",
            description=(
                f"The previous deviation {previous.accel_value_deviation} "
                f"(iteration {len(computed) - 2}) "
                f"is smaller than the current deviation {last.accel_value_deviation}, "
                "showing that the accelerated sequence is diverging."
            ),
        )
    return None


def monotone_accel_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Monotone acceleration detection (error magnitude unchanged).

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return None
    last, previous = computed[-1], computed[-2]
    
    if previous.accel_value_deviation == last.accel_value_deviation:
        return EventData(
            name="monotone_accel",
            description=(
                f"The deviation {previous.accel_value_deviation} "
                f"from iteration {len(computed) - 2} "
                f"matches the current deviation {last.accel_value_deviation}, "
                "indicating a monotone progression without change in magnitude."
            ),
        )
    return None


def sign_changed_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Sign change detection in acceleration errors (oscillation).

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return None
    last, previous = computed[-1], computed[-2]

    try:
        # Check if the error sign changed. 
        # We use type(0) to get a zero of the appropriate type (e.g. Arb(0) or 0.0).
        # Note: This may raise TypeError for complex numbers as < is not defined.
        if previous.accel_error * last.accel_error < type(last.accel_error)(0):
            return EventData(
                name="sign_changed",
                description=(
                    f"The error sign changed between iterations "
                    f"{len(computed) - 2} ({previous.accel_error}) "
                    f"and {len(computed) - 1} ({last.accel_error})"
                ),
            )
    except TypeError:
        # Comparison not supported (e.g., complex numbers)
        pass
        
    return None


def second_diff_growth_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Second difference growth detection.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 3:
        return None

    last = computed[-1]
    prev = computed[-2]
    prev2 = computed[-3]

    diff1 = abs(last.accel_value - prev.accel_value)
    diff2 = abs(prev.accel_value - prev2.accel_value)

    if diff1 >= diff2:
        return EventData(
            name="second_diff_growth",
            description=(
                f"The change |A[{len(computed) - 1}] − A[{len(computed) - 2}]| = {diff1} "
                f"is greater than or equal to "
                f"|A[{len(computed) - 2}] − A[{len(computed) - 3}]| = {diff2}, "
                "showing that second-order differences are not decreasing."
            ),
        )
    return None


EVENT_METHODS = {
    EventType.SLOW_ACCEL: slow_accel_method,
    EventType.MONOTONE_ACCEL: monotone_accel_method,
    EventType.DIVERGENT_ACCEL: divergent_accel_method,
    EventType.SIGN_CHANGED: sign_changed_method,
    EventType.SECOND_DIFF: second_diff_growth_method,
}
