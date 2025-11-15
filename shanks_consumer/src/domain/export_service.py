from typing import Any, Protocol, Sequence

from src.domain.trial_result import TrialResult


class ExportService(Protocol):
    def export(self, results: Sequence[TrialResult], **kwargs: Any) -> None: ...
