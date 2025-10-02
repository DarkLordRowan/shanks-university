"""
Parameter configuration module for series acceleration experiments.

This module provides classes and functions to define and load parameters
for numerical series and acceleration methods from various sources including
JSON files, CSV files, and direct Python module references.
"""

from abc import ABC, abstractmethod
from collections.abc import Callable
from dataclasses import dataclass
from functools import cached_property
from typing import Any, Iterable, Mapping, TypeAlias

import pyshanks as ps

NumericArg: TypeAlias = float | ps.Arb
SeriesData: TypeAlias = ps.ArraySeriesF64 | ps.ArraySeriesArb
SeriesExc: TypeAlias = (
    type[ps.SeriesBaseF64] | type[ps.SeriesBaseArb] | Callable[..., SeriesData]
)
AccelExc: TypeAlias = type[ps.SeriesAccelerationF64 | ps.SeriesAccelerationArb]


class BaseSeriesParam(ABC):
    """Abstract base class for series parameter configurations."""

    @cached_property
    @abstractmethod
    def series_name(self) -> str:
        ...

    @cached_property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[NumericArg]]:
        ...

    @cached_property
    @abstractmethod
    def executable(
        self,
    ) -> SeriesExc:
        ...

    def __str__(self) -> str:
        return f"{self.__class__.__name__}({self.series_name})"

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}(series_name={self.series_name!r})"


@dataclass
class SeriesParamJSON(BaseSeriesParam):
    """Series parameters loaded from JSON configuration.

    Attributes:
        name: Name of the series function in pyshanks module.
        args: Optional iterable of tuple of floats arguments for the series function.
    """

    name: str
    args: Mapping[str, Iterable[float | ps.Arb]]

    @cached_property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

    @cached_property
    def series_name(self):
        """Implementation of abstract method - returns the series name."""
        return self.name

    @cached_property
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(ps, self.name)


@dataclass
class SeriesParamModule(BaseSeriesParam):
    """Series parameters using direct Python module references.

    Attributes:
        caller: SeriesBase class or callable that generates the series.
        x: Optional iterable of integer arguments for the series function.
    """

    caller: type[ps.SeriesBaseF64 | ps.SeriesBaseArb]
    args: Mapping[str, Iterable[float | ps.Arb]]

    def __init__(
        self,
        caller: type[ps.SeriesBaseF64 | ps.SeriesBaseArb],
        **kwargs,
    ):
        """Initialize with direct caller reference and optional kwargs.

        Args:
            caller: SeriesAcceleration class reference.
            n: Iterable of integer n values.
            m: Iterable of integer m values.
            **kwargs: Additional initialization arguments.
        """
        self.caller = caller
        self.args = kwargs
        super().__init__()

    @cached_property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

    @cached_property
    def series_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @cached_property
    def executable(self):
        """Implementation of abstract method - returns the caller directly."""
        return self.caller


@dataclass
class SeriesParamCSV(BaseSeriesParam):
    """Series parameters loaded from CSV file data.

    Attributes:
        location: Path to the CSV file.
        row: Row number in the CSV file (1-indexed).
        data: ArraySeries containing the numerical data from the CSV row.
    """

    source_name: str
    row: int
    data: SeriesData

    @cached_property
    def arguments(self):
        """Implementation of abstract method - returns dummy arguments."""
        return {}

    @cached_property
    def series_name(self):
        """Implementation of abstract method - generates name from filename and row."""
        return f"{self.source_name}#{self.row}"

    @cached_property
    def executable(self):
        """Implementation of abstract method - returns lambda with pre-loaded data."""
        return lambda _: self.data


class BaseAccelParam(ABC):
    """Abstract base class for acceleration method parameters."""

    @cached_property
    @abstractmethod
    def accel_name(self) -> str:
        ...

    @cached_property
    @abstractmethod
    def executable(
        self,
    ) -> AccelExc:
        ...

    @cached_property
    @abstractmethod
    def n_values(self) -> Iterable[int]:
        ...

    @cached_property
    @abstractmethod
    def m_values(self) -> Iterable[int]:
        ...

    @cached_property
    @abstractmethod
    def additional_args(self) -> dict[str, Iterable[Any]]:
        ...

    def __str__(self) -> str:
        return f"{self.__class__.__name__}({self.accel_name})"

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}(accel_name={self.accel_name!r})"


@dataclass
class StandardAccelParam(BaseAccelParam):
    """Base class for acceleration parameters with standard n and m values.

    Attributes:
        n: Iterable of integer n values for the acceleration method.
        m: Iterable of integer m values for the acceleration method.
    """

    n: Iterable[int]
    m: Iterable[int]

    @cached_property
    def n_values(self):
        """Implementation of abstract method - returns n values."""
        return self.n

    @cached_property
    def m_values(self):
        """Implementation of abstract method - returns m values."""
        return self.m


@dataclass
class AccelParamJSON(StandardAccelParam):
    """Acceleration parameters loaded from JSON configuration.

    Attributes:
        name: Name of the acceleration method in pyshanks module.
        n: Iterable of integer n values.
        m: Iterable of integer m values.
        init_args: Optional dictionary of initialization arguments.
    """

    name: str
    n: Iterable[int]
    m: Iterable[int]
    init_args: Mapping[str, Iterable[Any]]

    def __post_init__(self):
        """Post-initialization processing for complex argument types."""
        self.expanded_init_args = {}

        if not self.init_args:
            return

        ENUM_MAPPINGS = {
            "remainder": ps.RemainderType,
            "numerator": ps.NumeratorType,
        }

        for key, value in self.init_args.items():
            if key in ENUM_MAPPINGS:
                enum_type = ENUM_MAPPINGS[key]
                self.expanded_init_args[key] = [
                    getattr(enum_type, str(v)) for v in value
                ]
            else:
                self.expanded_init_args[key] = value

    @cached_property
    def accel_name(self):
        """Implementation of abstract method - returns method name."""
        return self.name

    @cached_property
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(ps, self.name)

    @cached_property
    def additional_args(self):
        """Implementation of abstract method - returns expanded init args."""
        return self.expanded_init_args


@dataclass
class AccelParamModule(StandardAccelParam):
    """Acceleration parameters using direct Python module references.

    Attributes are dynamically set through kwargs in the constructor.
    """

    def __init__(
        self,
        caller: AccelExc,
        n: Iterable[int],
        m: Iterable[int],
        **kwargs,
    ):
        """Initialize with direct caller reference and optional kwargs.

        Args:
            caller: SeriesAcceleration class reference.
            n: Iterable of integer n values.
            m: Iterable of integer m values.
            **kwargs: Additional initialization arguments.
        """
        self.caller = caller
        self.init_args = kwargs
        super().__init__(n, m)

    @cached_property
    def accel_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @cached_property
    def executable(self):
        """Implementation of abstract method - returns the caller directly."""
        return self.caller

    @cached_property
    def additional_args(self):
        """Implementation of abstract method - returns init args or empty dict."""
        return self.init_args or {}
