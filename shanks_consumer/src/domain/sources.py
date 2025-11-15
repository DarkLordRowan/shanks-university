from typing import Iterable, Protocol, runtime_checkable

from src.domain.params import BaseAccelParam, BaseSeriesParam
from src.domain.precision import PrecisionType


@runtime_checkable
class SeriesParamSource(Protocol):
    def load(self, precision: PrecisionType) -> Iterable[BaseSeriesParam]: ...


@runtime_checkable
class AccelParamSource(Protocol):
    def load(self, precision: PrecisionType) -> Iterable[BaseAccelParam]: ...
