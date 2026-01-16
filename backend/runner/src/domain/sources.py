"""
Protocol definitions for sources of series and acceleration parameters.
Author: Yadrentsev I. M.
"""

from typing import Iterable, Protocol, runtime_checkable

from src.domain.params import BaseAccelParam, BaseSeriesParam
from src.domain.precision import PrecisionType
from src.config.model import NoiseConfig


@runtime_checkable
class SeriesParamSource(Protocol):
    """Protocol for sources of series parameters.

    Designs a protocol for loading series parameters based on a given precision.
    """

    def load(self, precision: PrecisionType) -> Iterable[BaseSeriesParam]: ...

    def load_noises(self) -> list[NoiseConfig]: ...


@runtime_checkable
class AccelParamSource(Protocol):
    """Protocol for sources of acceleration parameters.

    Designs a protocol for loading acceleration parameters based on a given precision.
    """

    def load(self, precision: PrecisionType) -> Iterable[BaseAccelParam]: ...
