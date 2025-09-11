"""
Parameter configuration module for series acceleration experiments.

This module provides classes and functions to define and load parameters
for numerical series and acceleration methods from various sources including
JSON files, CSV files, and direct Python module references.
"""

from dataclasses import dataclass
import pathlib
import json
import pyshanks
import csv
from abc import ABC, abstractmethod
from typing import Iterable
from collections.abc import Callable


class BaseSeriesParam(ABC):
    """Abstract base class for series parameter configurations."""

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> Iterable[int] | None: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[pyshanks.SeriesBase] | Callable[..., pyshanks.ArraySeries]: ...


@dataclass
class SeriesParamJSON(BaseSeriesParam):
    """Series parameters loaded from JSON configuration.

    Attributes:
        name: Name of the series function in pyshanks module.
        x: Optional iterable of integer arguments for the series function.
    """

    name: str
    x: Iterable[int] | None = None

    @property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.x

    @property
    def series_name(self):
        """Implementation of abstract method - returns the series name."""
        return self.name

    @property
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(pyshanks, self.name)


@dataclass
class SeriesParamModule(BaseSeriesParam):
    """Series parameters using direct Python module references.

    Attributes:
        caller: SeriesBase class or callable that generates the series.
        x: Optional iterable of integer arguments for the series function.
    """

    caller: type[pyshanks.SeriesBase]
    x: Iterable[int] | None = None

    @property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.x

    @property
    def series_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @property
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

    location: pathlib.Path
    row: int
    data: pyshanks.ArraySeries

    @property
    def arguments(self):
        """Implementation of abstract method - returns dummy arguments."""
        return [0]

    @property
    def series_name(self):
        """Implementation of abstract method - generates name from filename and row."""
        return f"{self.location.name}#{self.row}"

    @property
    def executable(self):
        """Implementation of abstract method - returns lambda with pre-loaded data."""
        return lambda _: self.data


class BaseAccelParam(ABC):
    """Abstract base class for acceleration method parameters."""

    @property
    @abstractmethod
    def accel_name(self) -> str: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[pyshanks.SeriesAcceleration]: ...

    @property
    @abstractmethod
    def n_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def m_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def additional_args(self) -> dict: ...


@dataclass
class StandardAccelParam(BaseAccelParam):
    """Base class for acceleration parameters with standard n and m values.

    Attributes:
        n: Iterable of integer n values for the acceleration method.
        m: Iterable of integer m values for the acceleration method.
    """

    n: Iterable[int]
    m: Iterable[int]

    @property
    def n_values(self):
        """Implementation of abstract method - returns n values."""
        return self.n

    @property
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
    init_args: dict | None = None

    def __post_init__(self):
        """Post-initialization processing for complex argument types."""
        self.expanded_init_args = {}
        if self.init_args:
            for key, value in self.init_args.items():
                if isinstance(value, dict):
                    self.expanded_init_args[key] = getattr(
                        pyshanks, value["type"]
                    )(value["value"])
                else:
                    self.expanded_init_args[key] = value

    @property
    def accel_name(self):
        """Implementation of abstract method - returns method name."""
        return self.name

    @property
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(pyshanks, self.name)

    @property
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
        caller: type[pyshanks.SeriesAcceleration],
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

    @property
    def accel_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @property
    def executable(self):
        """Implementation of abstract method - returns the caller directly."""
        return self.caller

    @property
    def additional_args(self):
        """Implementation of abstract method - returns init args or empty dict."""
        return self.init_args or {}


def get_series_params_from_json(
    json_location: pathlib.Path,
) -> list[SeriesParamJSON]:
    """Load series parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.

    Returns:
        List of SeriesParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f)

    series_list = []
    for series_data in data["series"]:
        x_value = series_data.get("x", None)
        if x_value is not None and not isinstance(x_value, list):
            x_value = [x_value]

        series_list.append(
            SeriesParamJSON(
                series_data["name"],
                x=x_value,
            )
        )

    return series_list


def get_accel_params_from_json(
    json_location: pathlib.Path,
) -> list[AccelParamJSON]:
    """Load acceleration parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.

    Returns:
        List of AccelParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f)
    methods_list = []
    for method_data in data["methods"]:
        n_value = method_data["n"]
        m_value = method_data["m"]

        if not isinstance(n_value, list):
            n_value = [n_value]
        if not isinstance(m_value, list):
            m_value = [m_value]

        methods_list.append(
            AccelParamJSON(
                name=method_data["name"],
                n=n_value,
                m=m_value,
                init_args=method_data.get("args", {}),
            )
        )

    return methods_list


def get_series_params_from_csv(
    csv_location: pathlib.Path,
) -> Iterable[SeriesParamCSV]:
    """Load series parameters from a CSV file.

    Each row in the CSV file becomes a separate series parameter.

    Args:
        csv_location: Path to the CSV file.

    Returns:
        Iterable of SeriesParamCSV objects, one for each row in the CSV.

    Raises:
        FileNotFoundError: If the CSV file doesn't exist.
    """
    with open(csv_location, encoding="utf-8") as f:
        return [
            SeriesParamCSV(
                location=csv_location,
                row=i,
                data=pyshanks.ArraySeries(
                    list(map(float, row))
                ),  # TODO possible better precision conversion
            )
            for i, row in enumerate(csv.reader(f), 1)
        ]
