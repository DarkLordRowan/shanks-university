from dataclasses import asdict, dataclass
from enum import Enum

from src.domain.trial_result import ComputedTrialResult
from src.logger import logged_debug


class EventType(Enum):
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


@dataclass
class EventData:
    name: str
    description: str
    data: dict


@logged_debug
def slow_accel_method(computed: list[ComputedTrialResult]) -> dict | None:
    if len(computed) < 1:
        return
    last = computed[-1]
    if last.accel_value_deviation < last.partial_sum_deviation:
        return asdict(
            EventData(
                name="slow_accel",
                description=f"{last.accel_value_deviation}\n"
                " is lesser than partial_sum_deviation"
                f"{last.partial_sum_deviation}",
                data={"computed_index": len(computed) - 1},
            )
        )


@logged_debug
def divergent_accel_method(computed: list[ComputedTrialResult]) -> dict | None:
    if len(computed) < 2:
        return
    last, previous = computed[-1], computed[-2]
    if previous.accel_value_deviation < last.accel_value_deviation:
        return asdict(
            EventData(
                name="devirgent",
                description=f"{previous.accel_value_deviation}\n"
                f"from a previous iteration {len(computed) - 2}\n"
                f"is lesser than current accel_value_deviation"
                f"{last.accel_value_deviation}",
                data={"computed_index": len(computed) - 1},
            )
        )


@logged_debug
def monotone_accel_method(computed: list[ComputedTrialResult]) -> dict | None:
    if len(computed) < 2:
        return
    last, previous = computed[-1], computed[-2]
    if previous.accel_value_deviation == last.accel_value_deviation:
        return asdict(
            EventData(
                name="monotone_accel",
                description=f"{previous.accel_value_deviation}\n"
                f"from a previous iteration {len(computed) - 2}\n"
                f"is lesser than current accel_value_deviation"
                f"{last.accel_value_deviation}",
                data={"computed_index": len(computed) - 1},
            )
        )


@logged_debug
def sign_changed_method(computed: list[ComputedTrialResult]) -> dict | None:
    if len(computed) < 2:
        return
    last, previous = computed[-1], computed[-2]
    if previous.accel_value_deviation * last.accel_value_deviation < 0:
        return asdict(
            EventData(
                name="sign_changed",
                description=f"{previous.accel_value_deviation}\n"
                f"from a previous iteration {len(computed) - 2}\n"
                f"is lesser than current accel_value_deviation"
                f"{last.accel_value_deviation}",
                data={"computed_index": len(computed) - 1},
            )
        )


@logged_debug
def second_diff_growth_method(
    computed: list[ComputedTrialResult],
) -> dict | None:
    if len(computed) < 3:
        return

    last = computed[-1]
    prev = computed[-2]
    prev2 = computed[-3]

    diff1 = abs(last.accel_value - prev.accel_value)
    diff2 = abs(prev.accel_value - prev2.accel_value)

    if diff1 >= diff2:
        return asdict(
            EventData(
                name="second_diff_growth",
                description=(
                    f"|A[{len(computed)-1}] - A[{len(computed)-2}]| = {diff1} "
                    f"is greater or equal than "
                    f"|A[{len(computed)-2}] - A[{len(computed)-3}]| = {diff2}"
                ),
                data={"computed_index": len(computed) - 1},
            )
        )


EVENT_METHODS = {
    EventType.SLOW_ACCEL: slow_accel_method,
    EventType.MONOTONE_ACCEL: monotone_accel_method,
    EventType.DIVERGENT_ACCEL: divergent_accel_method,
    EventType.SIGN_CHANGED: sign_changed_method,
    EventType.SECOND_DIFF: second_diff_growth_method,
}
