"""
Trial runner protocol definition.
Author: Yadrentsev I. M.
"""

from typing import Iterable, Iterator, Protocol

from src.domain.trial_result import TrialResult
from src.domain.params import (
    BaseSeriesParam,
    BaseAccelParam,
)


class TrialRunner(Protocol):
    """Protocol for running trials.

    Designs a protocol for executing trials based on combinations of series and acceleration parameters.
    """

    def run(
        self,
        combinations: Iterable[tuple[BaseSeriesParam, BaseAccelParam]],
    ) -> Iterator[list[TrialResult]]: ...
