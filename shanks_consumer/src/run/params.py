"""
Parameter configuration module for series acceleration experiments.

Provides helpers for loading/normalising series and acceleration parameters
for every precision exported by ``py.cpp`` (F32, F64, FLong, Arb, CF32, CF64, CFLong, CArb).
"""

import pathlib
from abc import ABC, abstractmethod
from collections.abc import Callable, Iterable, Sequence
from dataclasses import dataclass
from typing import Any, Generic, Mapping, override

import pyshanks as ps
from src.run.precision import (AccelProto, PrecisionType, SeriesBaseProto,
                               SeriesResultProto, TNum,
                               cast_natural_series_value, create_series_result)


class BaseSeriesParam[T]:
    """Abstract base class for series parameter configurations."""

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[T]]: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[SeriesBaseProto[TNum]] | Callable[..., SeriesResultProto[TNum]]: ...


@dataclass
class SeriesParamJSON(BaseSeriesParam[TNum]):
    name: str
    args: Mapping[str, Iterable[TNum]]
    precision: PrecisionType

    @property
    def arguments(self):
        return self.args

    @property
    def series_name(self):
        return self.name

    @property
    def executable(self):
        return getattr(ps, self.name)


@dataclass
class SeriesParamModule(BaseSeriesParam[TNum]):
    caller: type[SeriesBaseProto[TNum]]
    args: Mapping[str, Iterable[TNum]]

    def __init__(
        self,
        caller: type[SeriesBaseProto[TNum]],
        **kwargs,
    ):
        self.caller = caller
        self.args = kwargs
        super().__init__()

    @property
    def arguments(self):
        return self.args

    @property
    def series_name(self):
        return self.caller.__name__

    @property
    def executable(self):
        return self.caller


@dataclass
class SeriesParamCSV(BaseSeriesParam[TNum]):
    location: pathlib.Path
    row: int
    precision: PrecisionType
    raw_values: tuple[str, ...]

    @property
    def arguments(self):
        return {}

    @property
    def series_name(self):
        return f"{self.location.name}#{self.row}"

    @property
    @override
    def executable(self):
        def factory():
            return CSVSeriesWrapper(self.precision, self.raw_values)

        return factory


class CSVSeriesWrapper(Generic[TNum]):
    def __init__(self, precision: PrecisionType, raw_values: Sequence[str]):
        values = [cast_natural_series_value(precision, value) for value in raw_values]
        self.data = create_series_result(values, precision)
        self._sum = self.data.Sn[-1]
        self._precision = precision

    def generateSeries(
        self,
        x: TNum,
        vecSize: int,
        addTParameter: TNum,
        addKParameter: int,
    ) -> SeriesResultProto[TNum]:
        return self.data

    def get_sum(self) -> TNum:
        return self._sum

    def get_name(self) -> str:
        return f"CSVSeries_{self._precision.value}"


class BaseAccelParam(Generic[TNum], ABC):

    @property
    @abstractmethod
    def accel_name(self) -> str: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[AccelProto[TNum]]: ...

    @property
    @abstractmethod
    def n_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def m_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def additional_args(self) -> dict[str, Iterable[Any]]: ...


@dataclass
class StandardAccelParam(BaseAccelParam[TNum], ABC):
    n: Iterable[int]
    m: Iterable[int]

    @property
    @override
    def n_values(self):
        return self.n

    @property
    @override
    def m_values(self):
        return self.m


@dataclass
class AccelParamJSON(StandardAccelParam[TNum]):
    name: str
    n: Iterable[int]
    m: Iterable[int]
    init_args: Mapping[str, Iterable[Any]]
    precision: PrecisionType

    def __post_init__(self):
        self.expanded_init_args = {
            key: list(value) for key, value in (self.init_args or {}).items()
        }

    @property
    @override
    def accel_name(self):
        return self.name

    @property
    @override
    def executable(self):
        return getattr(ps, self.name)

    @property
    @override
    def additional_args(self):
        return self.expanded_init_args


@dataclass
class AccelParamModule(StandardAccelParam[TNum]):
    def __init__(
        self,
        caller: type[AccelProto[TNum]],
        n: Iterable[int],
        m: Iterable[int],
        **kwargs,
    ):
        self.caller = caller
        self.init_args = kwargs
        super().__init__(n, m)

    @property
    @override
    def accel_name(self):
        return self.caller.__name__

    @property
    @override
    def executable(self):
        return self.caller

    @property
    @override
    def additional_args(self):
        return self.init_args or {}
