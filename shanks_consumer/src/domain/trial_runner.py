from typing import Iterable, Iterator, Protocol

from src.domain.trial_result import TrialResult


class TrialRunner(Protocol):

    def run(self, combinations: Iterable[tuple]) -> Iterator[list[TrialResult]]: ...
