"""
Trial result data models.
Author: Yadrentsev I. M.
"""

from dataclasses import dataclass
from typing import Any, Mapping
from src.config.model import NoiseConfig


@dataclass
class EventData:
    """Represents an event that occurred during trial computation."""

    name: str
    description: str


@dataclass
class ComputedTrialResult:
    """Represents the result of a single computation within a trial."""

    n: int
    series_value: Any
    partial_sum: Any
    partial_sum_deviation: Any
    accel_value: Any
    accel_value_deviation: Any
    events: list[EventData]


@dataclass
class ErrorTrialResult:
    """Represents an error that occurred during trial execution."""

    description: str | None
    debug: str | None = None
    data: Mapping[str, Any] | None = None


NoErrorTrialResult = None


@dataclass
class SeriesTrialResult:
    """Represents the series parameters used in a trial."""

    id: int
    name: str
    lim: Any
    arguments: Mapping[str, Any]


@dataclass
class AccelTrialResult:
    """Represents the acceleration parameters used in a trial."""

    name: str
    m_value: int
    additional_args: Mapping[str, str]


@dataclass
class EventDataTrialResult:
    """Represents an event detected in the trial results."""

    computed_index: int
    description: str


@dataclass
class TrialResult:
    """Represents the complete result of a trial execution."""

    series: SeriesTrialResult
    accel: AccelTrialResult
    computed: list[ComputedTrialResult]
    noise: NoiseConfig | None = None
    error: ErrorTrialResult | None = None
    stack_id: str | None = None