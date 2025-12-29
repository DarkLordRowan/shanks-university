from typing import Protocol, Sequence

from src.domain.trial_result import TrialResult


class DataSerializer(Protocol):
    def to_dict(self, results: Sequence[TrialResult]) -> Sequence[dict]: ...
