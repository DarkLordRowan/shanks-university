"""
Parameter configuration module for series acceleration experiments.

This module provides classes and functions to define and load parameters
for numerical series and acceleration methods from various sources including
JSON files, CSV files, and direct Python module references.
"""

import inspect
from dataclasses import dataclass
import pathlib
import json
import pyshanks
import csv
from abc import ABC, abstractmethod
from typing import Iterable, Any, Mapping
from collections.abc import Callable


def autowrap(x: Any) -> Iterable[Any]:
    if x is not None and (isinstance(x, str) or not isinstance(x, Iterable)):
        return [x]
    return x


class BaseSeriesParam(ABC):
    """Abstract base class for series parameter configurations."""

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[float]]: ...

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
        args: Optional iterable of tuple of floats arguments for the series function.
    """

    name: str
    args: Mapping[str, Iterable[float]]

    @property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

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
    args: Mapping[str, Iterable[float]]

    def __init__(
        self,
        caller: type[pyshanks.SeriesBase],
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

    @property
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

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
        return {}

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
    def additional_args(self) -> dict[str, Iterable[Any]]: ...


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
    init_args: Mapping[str, Iterable[Any]]

    def __post_init__(self):
        """Post-initialization processing for complex argument types."""
        self.expanded_init_args = {}
        if self.init_args:
            for key, value in self.init_args.items():
                value = autowrap(value)

                def value_as_variants_of(of):
                    res = []
                    for v in value:
                        res.append(getattr(of, v))
                    return res

                if key == "remainder":
                    self.expanded_init_args[key] = value_as_variants_of(
                        pyshanks.RemainderType
                    )
                elif key == "numerator":
                    self.expanded_init_args[key] = value_as_variants_of(
                        pyshanks.NumeratorType
                    )
                else:
                    self.expanded_init_args[key] = autowrap(value)

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
    return load_series_params_from_data(data)


def load_series_params_from_data(
    data: dict,
) -> list[SeriesParamJSON]:
    series_list: list[SeriesParamJSON] = []
    for series_data in data["series"]:
        args = series_data.get("args", {})
        args = (
            {"x": map(float, autowrap(args))}
            if not isinstance(args, dict)
            else {
                str(key): map(float, autowrap(value))
                for key, value in args.items()
            }
        )
        series_list.append(
            SeriesParamJSON(name=series_data.get("name"), args=args)
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
    return load_accel_params_from_data(data)


def load_accel_params_from_data(
    data: dict,
) -> list[AccelParamJSON]:
    methods_list: list[AccelParamJSON] = []
    for method_data in data["methods"]:
        n_value = autowrap(method_data["n"])
        m_value = autowrap(method_data["m"])
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


def _is_concrete_subclass(cls: type, base: type) -> bool:
    """
    Return ``True`` if *cls* is a non‑abstract subclass of *base*.
    """
    return (
        inspect.isclass(cls)
        and issubclass(cls, base)
        and cls is not base
        and not inspect.isabstract(cls)
    )


def demo_all_synthetic_series(
    x: float | Iterable[float],
) -> list[BaseSeriesParam]:
    series_params: list[BaseSeriesParam] = []

    for name, cls in inspect.getmembers(pyshanks, inspect.isclass):
        if (
            _is_concrete_subclass(cls, pyshanks.SeriesBase)
            and name != "ArraySeries"
        ):
            series_params.append(
                SeriesParamModule(
                    caller=cls,
                    x=autowrap(x),
                )
            )
    return series_params


def all_accel(
    n: int | Iterable[int],
    m: int | Iterable[int],
    extra_args: dict[str, Any] | None = None,
) -> list[BaseAccelParam]:
    extra_args = extra_args or {}

    accel_params: list[BaseAccelParam] = []

    for _, cls in inspect.getmembers(pyshanks, inspect.isclass):
        if _is_concrete_subclass(cls, pyshanks.SeriesAcceleration):
            kwargs: dict[str, Any] = dict(extra_args.get(cls.__name__, {}))
            accel_params.append(
                AccelParamModule(
                    caller=cls,
                    n=autowrap(n),
                    m=autowrap(m),
                    **kwargs,
                )
            )
    return accel_params
