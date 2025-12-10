from typing import Iterable, Iterator, Protocol, Any

from src.domain.trial_result import TrialResult
from src.domain.params import (
    BaseSeriesParam,
    BaseAccelParam,
    PrecisionType,
    SeriesResultProto,
    NumericLike,
)


class TrialRunner(Protocol):

    def run(
        self,
        combinations: Iterable[tuple[BaseSeriesParam, BaseAccelParam]],
        pregen_series: (
            dict[
                tuple[str, PrecisionType, tuple[tuple[str, Any], ...]],
                tuple[SeriesResultProto[NumericLike], NumericLike],
            ]
            | None
        ) = None,
    ) -> Iterator[list[TrialResult]]: ...
