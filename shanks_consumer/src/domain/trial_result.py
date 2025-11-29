from dataclasses import dataclass
from typing import Any, Mapping


@dataclass
class EventData:
    name: str
    description: str


@dataclass
class ComputedTrialResult:
    n: int
    series_value: Any
    partial_sum: Any
    partial_sum_deviation: Any
    accel_value: Any
    accel_value_deviation: Any
    events: list[EventData]


@dataclass
class ErrorTrialResult:
    description: str | None
    debug: str | None = None
    data: Mapping[str, Any] | None = None


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

    @property
    def failed_iteration(self) -> Any:
        if not self.error:
            return "ok"
        return self.error.data.get("n", 0)
