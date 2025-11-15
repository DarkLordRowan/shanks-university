from enum import Enum
from typing import Any, Protocol

from src.domain.trial_result import TrialResult


class ResultView(Protocol):
    def transform(self, result: TrialResult) -> Any: ...


class ResultViewKind(Enum):
    FULL = "full"
    SHORT = "short"
    COMPLETION = "completion"
    ERRORS_ONLY = "errors_only"
