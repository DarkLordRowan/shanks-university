"""
Event definitions and methods for detecting specific behaviors in trial results.
Author: Shevyrov A.N., Yadrentsev I. M.
"""

from enum import Enum

from src.domain.trial_result import ComputedTrialResult, EventData


class EventType(Enum):
    """Enumeration of event types detected in trial results."""

    SLOW_ACCEL = "slow_accel"
    """Method is slower than partial sums.
    """
    MONOTONE_ACCEL = "monotone"
    """Method is monotone.
    """
    DIVERGENT_ACCEL = "divergent"
    """Method is divergent.
    """
    SIGN_CHANGED = "sign_changed"
    """Sign is changed.
    """
    SECOND_DIFF = "second_diff"
    """|Aₖ − Aₖ₋₁| ≥ |Aₖ₋₁ − Aₖ₋₂|
    """


def slow_accel_method(computed: list[ComputedTrialResult]) -> EventData | None:
    """Detects if the acceleration method is slower than the baseline partial sums.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 1:
        return
    last = computed[-1]
    if last.accel_value_deviation < last.partial_sum_deviation:
        return EventData(
            name="slow_accel",
            description=(
                f"The acceleration deviation {last.accel_value_deviation} "
                f"is smaller than the partial-sum deviation {last.partial_sum_deviation}, "
                "indicating slower improvement than the baseline sequence."
            ),
        )


def divergent_accel_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Detects if the acceleration method is diverging.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return
    last, previous = computed[-1], computed[-2]
    if previous.accel_value_deviation < last.accel_value_deviation:
        return EventData(
            name="divergent",
            description=(
                f"The previous deviation {previous.accel_value_deviation} "
                f"(iteration {len(computed) - 2}) "
                f"is smaller than the current deviation {last.accel_value_deviation}, "
                "showing that the accelerated sequence is diverging."
            ),
        )


def monotone_accel_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Monotone acceleration detection.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return
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


def sign_changed_method(
    computed: list[ComputedTrialResult],
) -> EventData | None:
    """Sign change detection in acceleration deviations.

    :param computed: _computed trial results
    :type computed: list[ComputedTrialResult]
    :return: _detected event data or None
    :rtype: EventData | None
    """
    if len(computed) < 2:
        return
    last, previous = computed[-1], computed[-2]

    if previous.accel_value_deviation * last.accel_value_deviation < type(
        last.accel_value_deviation
    )(0):
        return EventData(
            name="sign_changed",
            description=(
                f"The deviation changed sign between iterations "
                f"{len(computed) - 2} ({previous.accel_value_deviation}) "
                f"and {len(computed) - 1} ({last.accel_value_deviation})"
            ),
        )


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
        return

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


EVENT_METHODS = {
    EventType.SLOW_ACCEL: slow_accel_method,
    EventType.MONOTONE_ACCEL: monotone_accel_method,
    EventType.DIVERGENT_ACCEL: divergent_accel_method,
    EventType.SIGN_CHANGED: sign_changed_method,
    EventType.SECOND_DIFF: second_diff_growth_method,
}
