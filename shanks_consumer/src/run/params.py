"""
Parameter configuration module for series acceleration experiments.

Provides helpers for loading/normalising series and acceleration parameters
for every precision exported by ``py.cpp`` (F32, F64, FLong, Arb, CF32, CF64, CFLong, CArb).
"""

from __future__ import annotations

import csv
import inspect
import json
import pathlib
from abc import ABC, abstractmethod
from collections.abc import Callable, Iterable, Sequence
from dataclasses import dataclass
from enum import Enum
from typing import Any, Generic, Mapping, Protocol, TypeVar, runtime_checkable, override

import pyshanks as ps


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
    def generateSeries(
        self,
        x: TNum,
        vecSize: int,
        addTParameter: TNum,
        addKParameter: int,
    ) -> SeriesResultProto[TNum]: ...

    def get_sum(self) -> TNum: ...


class AccelProto(Protocol[TNum]):
    def __call__(self, n: int, m: int, series: SeriesResultProto[TNum]) -> TNum: ...


class PrecisionType(Enum):
    F32 = "F32"
    F64 = "F64"
    FLONG = "FLong"
    ARB = "Arb"
    CF32 = "CF32"
    CF64 = "CF64"
    CFLONG = "CFLong"
    CARB = "CArb"


REAL_PRECISIONS = {
    PrecisionType.F32,
    PrecisionType.F64,
    PrecisionType.FLONG,
}

COMPLEX_PRECISIONS = {
    PrecisionType.CF32,
    PrecisionType.CF64,
    PrecisionType.CFLONG,
    PrecisionType.CARB,
}

_COMPLEX_CLASS = {
    PrecisionType.CF32: ps.CF32,
    PrecisionType.CF64: ps.CF64,
    PrecisionType.CFLONG: ps.CFLong,
    PrecisionType.CARB: ps.CArb,
}

_SERIES_RESULT_CLASS = {
    PrecisionType.F32: ps.SeriesResultF32,
    PrecisionType.F64: ps.SeriesResultF64,
    PrecisionType.FLONG: ps.SeriesResultFLong,
    PrecisionType.ARB: ps.SeriesResultArb,
    PrecisionType.CF32: ps.SeriesResultCF32,
    PrecisionType.CF64: ps.SeriesResultCF64,
    PrecisionType.CFLONG: ps.SeriesResultCFLong,
    PrecisionType.CARB: ps.SeriesResultCArb,
}

_REAL_SUBTYPE_FOR_PRECISION = {
    PrecisionType.CF32: PrecisionType.F32,
    PrecisionType.CF64: PrecisionType.F64,
    PrecisionType.CFLONG: PrecisionType.FLONG,
    PrecisionType.CARB: PrecisionType.ARB,
}


def is_complex_precision(precision: PrecisionType) -> bool:
    return precision in COMPLEX_PRECISIONS


def zero_for_precision(precision: PrecisionType):
    if precision == PrecisionType.ARB:
        return ps.Arb(0)
    if precision in COMPLEX_PRECISIONS:
        real_zero = cast_real_subtype_value(precision, 0)
        cls = _COMPLEX_CLASS[precision]
        return cls(real_zero)
    return 0.0


def one_for_precision(precision: PrecisionType):
    if precision == PrecisionType.ARB:
        return ps.Arb(1)
    if precision in COMPLEX_PRECISIONS:
        real_one = cast_real_subtype_value(precision, 1)
        cls = _COMPLEX_CLASS[precision]
        return cls(real_one)
    return 1.0


def cast_precision_value(precision: PrecisionType, value: Any):
    if value is None or isinstance(value, bool):
        return value

    if precision in REAL_PRECISIONS:
        if isinstance(value, (int, float)):
            return float(value)
        try:
            return float(value)
        except (TypeError, ValueError):
            return value

    if precision == PrecisionType.ARB:
        if isinstance(value, ps.Arb):
            return value
        return ps.Arb(str(value))

    if precision in COMPLEX_PRECISIONS:
        cls = _COMPLEX_CLASS[precision]
        if isinstance(value, cls):
            return value
        if isinstance(value, complex):
            return cls(value.real, value.imag)
        if isinstance(value, (tuple, list)) and len(value) == 2:
            return cls(value[0], value[1])
        if precision == PrecisionType.CARB and not isinstance(value, ps.Arb):
            return cls(ps.Arb(str(value)))
        return cls(value)

    return value


def cast_real_subtype_value(precision: PrecisionType, value: Any):
    real_precision = _REAL_SUBTYPE_FOR_PRECISION.get(precision, precision)
    return cast_precision_value(real_precision, value)


def series_result_ctor_for_precision(precision: PrecisionType):
    return _SERIES_RESULT_CLASS[precision]


def autowrap(x: Any) -> Iterable[Any]:
    if x is None:
        return []

    if isinstance(x, dict) and all(k in x for k in ("start", "stop", "step")):
        start = _ensure_number(x["start"])
        stop = _ensure_number(x["stop"])
        step = _ensure_number(x["step"])
        return _generate_range_values(start, stop, step)

    if isinstance(x, (str, bytes, bytearray)):
        return [x]

    if isinstance(x, Iterable) and not isinstance(x, (dict, str, bytes, bytearray)):
        return [_ensure_number(value) for value in x]

    return [_ensure_number(x)]


def _ensure_number(value: Any) -> Any:
    if isinstance(value, bool):
        return value
    if isinstance(value, (int, float)):
        return value
    if isinstance(value, str):
        try:
            if any(sep in value for sep in (".", "e", "E")):
                return float(value)
            return int(value)
        except ValueError:
            return value
    try:
        return float(value)
    except (TypeError, ValueError):
        return value


def _generate_range_values(start: float, stop: float, step: float) -> list[float]:
    if step == 0:
        raise ValueError("Range specification has zero step")

    steps = int((stop - start) / step)
    return [start + i * step for i in range(steps)]


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
    name: str
    args: Mapping[str, Iterable[TNum]]
    precision: PrecisionType

    @property
    @override
    def arguments(self):
        return self.args

    @property
    @override
    def series_name(self):
        return self.name

    @property
    @override
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
    @override
    def arguments(self):
        return self.args

    @property
    @override
    def series_name(self):
        return self.caller.__name__

    @property
    @override
    def executable(self):
        return self.caller


@dataclass
class SeriesParamCSV(BaseSeriesParam[TNum]):
    location: pathlib.Path
    row: int
    data: SeriesResultProto[TNum]

    @property
    @override
    def arguments(self):
        return {}

    @property
    @override
    def series_name(self):
        return f"{self.location.name}#{self.row}"

    @property
    @override
    def executable(self):
        class CSVSeriesWrapper:
            def __init__(self, data: SeriesResultProto[TNum]):
                self.data = data
                self._sum = data.Sn[-1]

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

            def get_name(self):
                return f"CSVSeries_{self.data.__class__.__name__}"

        return lambda: CSVSeriesWrapper(self.data)


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


class XArbEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(
            o,
            (
                ps.Arb,
                ps.CArb,
                ps.CF32,
                ps.CF64,
                ps.CFLong,
            ),
        ):
            return str(o)
        return super().default(o)


def decoder_for_prec(precision: PrecisionType):
    if precision == PrecisionType.ARB:
        class _Decoder(json.JSONDecoder):
            def __init__(self, *args, **kwargs):
                super().__init__(parse_float=ps.Arb, parse_int=ps.Arb, *args, **kwargs)
        return _Decoder

    return None


def get_series_params_from_json(
    json_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64,
) -> list[SeriesParamJSON[Any]]:
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=decoder_for_prec(precision))
    return load_series_params_from_data(data, precision)


def load_series_params_from_data(
    data: Mapping[str, Any],
    precision: PrecisionType,
) -> list[SeriesParamJSON[Any]]:
    series_list: list[SeriesParamJSON[Any]] = []
    for series_data in data["series"]:
        args = series_data.get("args", {})
        processed_args = SeriesParamLoader._process_series_args(args, precision)
        series_list.append(
            SeriesParamJSON(
                name=series_data.get("name") + precision.value,
                args=processed_args,
                precision=precision,
            )
        )
    return series_list


def get_accel_params_from_json(
    json_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64,
) -> list[AccelParamJSON[Any]]:
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=decoder_for_prec(precision))
    return load_accel_params_from_data(data, precision)


def load_accel_params_from_data(
    data: Mapping[str, Any],
    precision: PrecisionType,
) -> list[AccelParamJSON[Any]]:
    methods_list: list[AccelParamJSON[Any]] = []

    for method_data in data["methods"]:
        n_value = [int(x) if not isinstance(x, int) else x for x in autowrap(method_data["n"])]
        m_value = [int(x) if not isinstance(x, int) else x for x in autowrap(method_data["m"])]

        args = {}
        for key, value in method_data.get("args", {}).items():
            key_str = str(key)
            values = list(autowrap(value))

            if key_str == "remainder":
                args[key_str] = [getattr(ps.RemainderType, str(v)) for v in values]
            elif key_str == "numerator":
                args[key_str] = [getattr(ps.NumeratorType, str(v)) for v in values]
            else:
                converted: list[Any] = []
                for item in values:
                    if isinstance(item, bool):
                        converted.append(item)
                    else:
                        converted.append(cast_real_subtype_value(precision, item))
                args[key_str] = converted

        methods_list.append(
            AccelParamJSON(
                name=method_data["name"] + precision.value,
                n=n_value,
                m=m_value,
                init_args=args,
                precision=precision,
            )
        )

    return methods_list


def get_series_params_from_csv(
    csv_location: pathlib.Path,
    precision: PrecisionType = PrecisionType.F64,
) -> list[SeriesParamCSV[Any]]:
    results: list[SeriesParamCSV[Any]] = []
    with open(csv_location, encoding="utf-8") as f:
        reader = csv.reader(f)
        for i, row in enumerate(reader, 1):
            an_vals = [_cast_csv_value(value, precision) for value in row]
            series_result = _create_series_result(an_vals, precision)
            results.append(
                SeriesParamCSV(
                    location=pathlib.Path(csv_location),
                    row=i,
                    data=series_result,
                )
            )
    return results


def _cast_csv_value(value: str, precision: PrecisionType):
    if precision in REAL_PRECISIONS:
        return float(value)
    if precision == PrecisionType.ARB:
        return ps.Arb(value)
    if precision in COMPLEX_PRECISIONS:
        cls = _COMPLEX_CLASS[precision]
        real_value = cast_real_subtype_value(precision, value)
        try:
            return cls(real_value)
        except TypeError:
            return cls(value)
    return value


def _create_series_result(values: list[Any], precision: PrecisionType):
    cumulative = []
    total = zero_for_precision(precision)
    for v in values:
        total = total + v
        cumulative.append(total)
    ctor = series_result_ctor_for_precision(precision)
    return ctor(cumulative, values)


class SeriesParamLoader:
    @staticmethod
    def from_json(
        json_location: pathlib.Path,
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[SeriesParamJSON]:
        return get_series_params_from_json(json_location, precision)

    @staticmethod
    def from_data(
        data: Mapping[str, Any],
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[SeriesParamJSON]:
        return load_series_params_from_data(data, precision)

    @staticmethod
    def from_csv(
        csv_location: pathlib.Path,
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[SeriesParamCSV]:
        return get_series_params_from_csv(csv_location, precision)

    @staticmethod
    def _process_series_args(args: Any, precision: PrecisionType) -> dict[str, Iterable[Any]]:
        if not args:
            return {}

        if not isinstance(args, dict):
            args = {"x": autowrap(args)}

        processed: dict[str, Iterable[Any]] = {}
        for key, value in args.items():
            key_str = str(key)
            if key_str in {"vecSize", "addKParameter", "m", "b"}:
                processed[key_str] = [int(v) for v in autowrap(value)]
            else:
                processed[key_str] = [
                    cast_precision_value(precision, v) for v in autowrap(value)
                ]
        return processed


class AccelParamLoader:
    @staticmethod
    def from_json(
        json_location: pathlib.Path,
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[AccelParamJSON]:
        return get_accel_params_from_json(json_location, precision)

    @staticmethod
    def from_data(
        data: Mapping[str, Any],
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[AccelParamJSON]:
        return load_accel_params_from_data(data, precision)


def _is_concrete_subclass(cls: type, base: type) -> bool:
    return (
        inspect.isclass(cls)
        and issubclass(cls, base)
        and cls is not base
        and not inspect.isabstract(cls)
    )


def _get_acceleration_base_class(precision: PrecisionType) -> type:
    if precision == PrecisionType.F32:
        return ps.SeriesAccelerationF32
    if precision == PrecisionType.F64:
        return ps.SeriesAccelerationF64
    if precision == PrecisionType.FLONG:
        return ps.SeriesAccelerationFLong
    if precision == PrecisionType.ARB:
        return ps.SeriesAccelerationArb
    if precision == PrecisionType.CF32:
        return ps.SeriesAccelerationCF32
    if precision == PrecisionType.CF64:
        return ps.SeriesAccelerationCF64
    if precision == PrecisionType.CFLONG:
        return ps.SeriesAccelerationCFLong
    if precision == PrecisionType.CARB:
        return ps.SeriesAccelerationCArb
    raise ValueError(f"Unsupported precision: {precision}")
