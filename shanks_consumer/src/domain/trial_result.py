from dataclasses import asdict, dataclass, field
from typing import Any, Callable, Mapping


@dataclass
class ComputedTrialResult:
    n: int
    series_value: Any
    partial_sum: Any
    partial_sum_deviation: Any
    accel_value: Any
    accel_value_deviation: Any


@dataclass
class ErrorTrialResult:
    description: str | None
    data: Mapping[str, Any]


NoErrorTrialResult = None


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
class EventTrialResult:
    event: str
    data: dict


@dataclass
class EventDataTrialResult:
    computed_index: int
    description: str


@dataclass
class TrialResult:
    series: SeriesTrialResult
    accel: AccelTrialResult
    computed: list[ComputedTrialResult]
    error: ErrorTrialResult | None = None
    stack_id: str | None = None
    events: list[EventTrialResult] | None = field(default=None, init=False, repr=False)

    def load_events(self) -> "TrialResult":
        if self.events is None:
            self.events = self._lazy_events()

        return self

    def _lazy_events(self) -> list[EventTrialResult]:
        def _slow_accel_method(result: TrialResult) -> dict | None:
            for i, compute in enumerate(result.computed):
                if compute.accel_value_deviation < compute.partial_sum_deviation:
                    return asdict(
                        EventDataTrialResult(
                            computed_index=i,
                            description=f"accel_value_deviation"
                            f"{compute.accel_value_deviation}\n"
                            " is lesser than partial_sum_deviation"
                            f"{compute.partial_sum_deviation}",
                        )
                    )
            return None

        def _divergent_accel_method(result: TrialResult) -> dict | None:
            for i in range(1, len(result.computed)):
                if (
                    result.computed[i - 1].accel_value_deviation
                    < result.computed[i].accel_value_deviation
                ):
                    return asdict(
                        EventDataTrialResult(
                            computed_index=i,
                            description=f"accel_value_deviation"
                            f"{result.computed[i - 1].accel_value_deviation}\n"
                            f"from a previous iteration {i - 1}\n"
                            f"is lesser than current accel_value_deviation"
                            f"{result.computed[i].accel_value_deviation}",
                        )
                    )
            return None

        _scan_methods: dict[str, Callable] = {
            "slow_accel_method": _slow_accel_method,
            "divergent_accel_method": _divergent_accel_method,
        }

        events = []

        for name, method in _scan_methods.items():
            event_data = method(self)
            if event_data:
                events.append(EventTrialResult(name, event_data))
        return events
