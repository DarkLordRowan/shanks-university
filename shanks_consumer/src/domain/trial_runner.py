from typing import Iterable, Iterator, Protocol

from src.domain.trial_result import TrialResult
from src.domain.params import (
    BaseSeriesParam,
    BaseAccelParam,
)

class TrialRunner(Protocol):

    def run(
        self,
        combinations: Iterable[tuple[BaseSeriesParam, BaseAccelParam]],
    ) -> Iterator[list[TrialResult]]: ...
