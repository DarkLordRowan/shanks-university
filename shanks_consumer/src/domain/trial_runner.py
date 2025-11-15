from typing import Iterable, Protocol

from src.domain.trial_result import TrialResult


class TrialRunner(Protocol):
    def run(self, combinations: Iterable[tuple]) -> list[TrialResult]: ...
