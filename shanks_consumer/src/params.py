"""
Parameter configuration module for series acceleration experiments.

This module provides classes and functions to define and load parameters
for numerical series and acceleration methods from various sources including
JSON files, CSV files, and direct Python module references.
"""
import inspect
from dataclasses import dataclass
from json.encoder import JSONEncoder
import pathlib
import json
import pyshanks as ps
import csv
from abc import ABC, abstractmethod
from typing import Iterable, Any, Mapping, Protocol, TypeVar, Sequence, Generic, runtime_checkable, Union, override
from collections.abc import Callable
from enum import Enum

# Protocol definitions for generic numeric types
@runtime_checkable
class NumericLike(Protocol):
    def __add__(self, other: Any) -> Any: ...
    def __sub__(self, other: Any) -> Any: ...
    def __abs__(self) -> Any: ...

TNum = TypeVar("TNum", bound=NumericLike)

class SeriesResultProto(Protocol[TNum]):
    Sn: Sequence[TNum]
    an: Sequence[TNum]

class SeriesBaseProto(Protocol[TNum]):
    def generateSeries(self, x: TNum, vecSize: int, addTParameter: TNum, addKParameter: int) -> SeriesResultProto[TNum]: ...
    def get_sum(self) -> TNum: ...

class AccelProto(Protocol[TNum]):
    def __call__(self, n: int, m: int, series: SeriesResultProto[TNum]) -> TNum: ...

class PrecisionType(Enum):
    F64 = "F64"
    ARB = "Arb"
    CARB = "CArb"

def autowrap(x: Any) -> Iterable[Any]:
    if x is not None and (isinstance(x, str) or not isinstance(x, Iterable)):
        return [x]
    if isinstance(x, dict):
        # ? consider as range value
        return [
            x["start"] + i * x["step"]
            for i in range(int((x["stop"] - x["start"]) / x["step"]))
        ]
    return x

class BaseSeriesParam(Generic[TNum], ABC):
    """Abstract base class for series parameter configurations."""

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[TNum]]: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[SeriesBaseProto[TNum]] | Callable[..., SeriesResultProto[TNum]]: ...


@dataclass
class SeriesParamJSON(BaseSeriesParam[TNum]):
    """Series parameters loaded from JSON configuration.

    Attributes:
        name: Name of the series function in pyshanks module.
        args: Optional iterable of tuple of floats arguments for the series function.
    """

    name: str
    args: Mapping[str, Iterable[TNum]]

    @property
    @override
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

    @property
    @override
    def series_name(self):
        """Implementation of abstract method - returns the series name."""
        return self.name

    @property
    @override
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(ps, self.name)


@dataclass
class SeriesParamModule(BaseSeriesParam[TNum]):
    """Series parameters using direct Python module references.

    Attributes:
        caller: SeriesBase class or callable that generates the series.
        x: Optional iterable of integer arguments for the series function.
    """

    caller: type[SeriesBaseProto[TNum]]
    args: Mapping[str, Iterable[TNum]]

    def __init__(
        self,
        caller: type[SeriesBaseProto[TNum]],
        **kwargs,
    ):
        """Initialize with direct caller reference and optional kwargs.

        Args:
            caller: SeriesBase class reference.
            **kwargs: Additional initialization arguments for generateSeries.
        """
        self.caller = caller
        self.args = kwargs
        super().__init__()

    @property
    @override
    def arguments(self):
        """Implementation of abstract method - returns the x arguments."""
        return self.args

    @property
    @override
    def series_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @property
    @override
    def executable(self):
        """Implementation of abstract method - returns the caller directly."""
        return self.caller


@dataclass
class SeriesParamCSV(BaseSeriesParam[TNum]):
    """Series parameters loaded from CSV file data.

    Attributes:
        location: Path to the CSV file.
        row: Row number in the CSV file (1-indexed).
        data: ArraySeries containing the numerical data from the CSV row.
    """

    location: pathlib.Path
    row: int
    data: SeriesResultProto[TNum]

    @property
    @override
    def arguments(self):
        """Implementation of abstract method - returns dummy arguments."""
        return {}

    @property
    @override
    def series_name(self):
        """Implementation of abstract method - generates name from filename and row."""
        return f"{self.location.name}#{self.row}"

    @property
    @override
    def executable(self):
        """Implementation of abstract method - returns lambda with pre-loaded data."""
        class CSVSeriesWrapper:
            def __init__(self, data: SeriesResultProto[TNum]):
                self.data = data
                self._sum = data.Sn[-1]

            def generateSeries(self, x: TNum, vecSize: int, addTParameter: TNum, addKParameter: int) -> SeriesResultProto[TNum]:
                return self.data

            def get_sum(self) -> TNum:
                return self._sum

            def get_name(self):
                return f"CSVSeries_{self.data.__class__.__name__}"

        return lambda: CSVSeriesWrapper(self.data)


class BaseAccelParam(Generic[TNum], ABC):
    """Abstract base class for acceleration method parameters."""

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
    """Base class for acceleration parameters with standard n and m values.

    Attributes:
        n: Iterable of integer n values for the acceleration method.
        m: Iterable of integer m values for the acceleration method.
    """

    n: Iterable[int]
    m: Iterable[int]

    @property
    @override
    def n_values(self):
        """Implementation of abstract method - returns n values."""
        return self.n

    @property
    @override
    def m_values(self):
        """Implementation of abstract method - returns m values."""
        return self.m


@dataclass
class AccelParamJSON(StandardAccelParam[TNum]):
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
                    self.expanded_init_args[key] = value_as_variants_of(ps.RemainderType)
                elif key == "numerator":
                    self.expanded_init_args[key] = value_as_variants_of(ps.NumeratorType)
                else:
                    self.expanded_init_args[key] = autowrap(value)

    @property
    @override
    def accel_name(self):
        """Implementation of abstract method - returns method name."""
        return self.name

    @property
    @override
    def executable(self):
        """Implementation of abstract method - gets executable from pyshanks."""
        return getattr(ps, self.name)

    @property
    @override
    def additional_args(self):
        """Implementation of abstract method - returns expanded init args."""
        return self.expanded_init_args


@dataclass
class AccelParamModule(StandardAccelParam[TNum]):
    """Acceleration parameters using direct Python module references.

    Attributes are dynamically set through kwargs in the constructor.
    """

    def __init__(
        self,
        caller: type[AccelProto[TNum]],
        n: Iterable[int],
        m: Iterable[int],
        **kwargs,
    ):
        """Initialize with direct caller reference and optional kwargs.

        Args:
            caller: SeriesAcceleration class reference.
            n: Iterable of integer n values.
            m: Iterable of integer m values.
            **kwargs: Additional initialization arguments for constructor.
        """
        self.caller = caller
        self.init_args = kwargs
        super().__init__(n, m)

    @property
    @override
    def accel_name(self):
        """Implementation of abstract method - returns caller's name."""
        return self.caller.__name__

    @property
    @override
    def executable(self):
        """Implementation of abstract method - returns the caller directly."""
        return self.caller

    @property
    @override
    def additional_args(self):
        """Implementation of abstract method - returns init args or empty dict."""
        return self.init_args or {}

class XArbEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, ps.Arb) or isinstance(o, ps.CArb):
            return str(o)
        return super().default(o)

def decoder_for_prec(prec: PrecisionType):
    if prec == PrecisionType.F64:
      return None

    def _parse(s: str):
        return ps.Arb(s) if prec == PrecisionType.ARB else ps.CArb(s)
    class _Decoder(json.JSONDecoder):
        def __init__(self, *args, **kwargs):
            super().__init__(parse_float=_parse, parse_int=_parse, *args, **kwargs)
    return _Decoder

def get_series_params_from_json(
    json_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64
) -> list[SeriesParamJSON[Any]]:
    """Load series parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.
        precision: Precision type to use (F64, Arb, or CArb).

    Returns:
        List of SeriesParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=decoder_for_prec(precision))
    return load_series_params_from_data(data, precision)


def load_series_params_from_data(
    data: dict,
    precision: PrecisionType,
) -> list[SeriesParamJSON[Any]]:
    series_list: list[SeriesParamJSON[Any]] = []
    for series_data in data["series"]:
        args = series_data.get("args", {})
        if not isinstance(args, dict):
            args = {"x": autowrap(args) }
        else:
            args = {
                str(key): autowrap(value)
                for key, value in args.items()
            }
        series_list.append(
            SeriesParamJSON(name=series_data.get("name") + precision.value, args=args)
        )
    return series_list


def get_accel_params_from_json(
    json_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64,
) -> list[AccelParamJSON[Any]]:
    """Load acceleration parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.
        precision: Precision type to use (F64, Arb, or CArb).

    Returns:
        List of AccelParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=decoder_for_prec(precision))
    return load_accel_params_from_data(data, precision)


def load_accel_params_from_data(
    data: dict,
    precision: PrecisionType,
) -> list[AccelParamJSON[Any]]:
    methods_list: list[AccelParamJSON[Any]] = []
    for method_data in data["methods"]:
        n_value = [int(x) if not isinstance(x, int) else x for x in autowrap(method_data["n"])]
        m_value = [int(x) if not isinstance(x, int) else x for x in autowrap(method_data["m"])]
        suffix = precision.value
        methods_list.append(
            AccelParamJSON(
                name=method_data["name"] + suffix,
                n=n_value,
                m=m_value,
                init_args=method_data.get("args", {}),
            )
        )
    return methods_list


def get_series_params_from_csv(
    csv_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64,
) -> Iterable[SeriesParamCSV[Any]]:
    """Load series parameters from a CSV file.

    Each row in the CSV file becomes a separate series parameter.

    Args:
        csv_location: Path to the CSV file.
        precision: Precision type to use (F64, Arb, or CArb).

    Returns:
        Iterable of SeriesParamCSV objects, one for each row in the CSV.

    Raises:
        FileNotFoundError: If the CSV file doesn't exist.
    """
    def _cumulative(vals: list[Any]) -> list[Any]:
        """Return a list of cumulative sums for the given iterable."""
        cum = []
        if precision == PrecisionType.F64:
            total = 0.0
        elif precision == PrecisionType.ARB:
            total = ps.Arb(0)
        elif precision == PrecisionType.CARB:
            total = ps.CArb(0)
        else:
            total = 0.0

        for v in vals:
            total = total + v
            cum.append(total)
        return cum

    def _parse_row_values(row: list[str]) -> list[Any]:
        """Parse CSV row values according to precision type."""
        if precision == PrecisionType.F64:
            return [float(val) for val in row]
        elif precision == PrecisionType.ARB:
            return [ps.Arb(val) for val in row]
        elif precision == PrecisionType.CARB:
            return [ps.CArb(val) for val in row]
        else:
            return [float(val) for val in row]

    def _create_series_result(an_vals: list[Any]) -> Any:
        """Create SeriesResult object based on precision type."""
        if precision == PrecisionType.F64:
            return ps.SeriesResultF64(_cumulative(an_vals), an_vals)
        elif precision == PrecisionType.ARB:
            return ps.SeriesResultArb(_cumulative(an_vals), an_vals)
        elif precision == PrecisionType.CARB:
            return ps.SeriesResultCArb(_cumulative(an_vals), an_vals)
        else:
            return ps.SeriesResultF64(_cumulative(an_vals), an_vals)

    with open(csv_location, encoding="utf-8") as f:
        return [
            SeriesParamCSV(
                location=pathlib.Path(csv_location),
                row=i,
                data=_create_series_result(an_vals),
            )
            for i, row in enumerate(csv.reader(f), 1)
            for an_vals in [_parse_row_values(row)]
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


def _get_acceleration_base_class(precision: PrecisionType) -> type:
    """Get the appropriate base acceleration class for the given precision."""
    if precision == PrecisionType.F64:
        return ps.SeriesAccelerationF64
    elif precision == PrecisionType.ARB:
        return ps.SeriesAccelerationArb
    elif precision == PrecisionType.CARB:
        return ps.SeriesAccelerationCArb
    else:
        return ps.SeriesAccelerationF64
