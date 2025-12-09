import re
from dataclasses import dataclass
from enum import Enum
from typing import Any, Optional, Protocol, Sequence, TypeVar, runtime_checkable

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


@dataclass
class PrecisionConfig:
    """Configuration for precision types with optional precision value for ARB/CARB."""

    type: PrecisionType
    precision: Optional[int] = None  # Only used for ARB/CARB types

    @classmethod
    def from_string(cls, value: str) -> "PrecisionConfig":
        """Parse precision string like 'CArb100', 'Arb50', 'F64' into PrecisionConfig."""
        value = value.strip()

        # Check for ARB/CARB with precision suffix (case-sensitive for actual enum values)
        match = re.match(r"^(CArb|Arb)(\d+)$", value)
        if match:
            type_str, precision_str = match.groups()
            if type_str == "CArb":
                return cls(PrecisionType.CARB, int(precision_str))
            elif type_str == "Arb":
                return cls(PrecisionType.ARB, int(precision_str))

        # Try uppercase version for backward compatibility (handles "arb50", "carb100")
        value_upper = value.upper()
        match_upper = re.match(r"^(CA?RB)(\d+)$", value_upper)
        if match_upper:
            type_str, precision_str = match_upper.groups()
            if type_str == "CARB":
                return cls(PrecisionType.CARB, int(precision_str))
            elif type_str == "ARB":
                return cls(PrecisionType.ARB, int(precision_str))

        # Handle standard precision types (case-insensitive for backward compatibility)
        # This handles: "F64", "f64", "CF32", "cf32", etc.
        try:
            precision_type = PrecisionType[value_upper]
            return cls(precision_type)
        except KeyError:
            # Try to match by enum value (case-insensitive)
            for precision in PrecisionType:
                if value_upper == precision.value.upper():
                    return cls(precision)

        raise ValueError(f"Unsupported precision: {value}")

    def __str__(self) -> str:
        """String representation for logging and display."""
        if self.precision is not None:
            return f"{self.type.value}{self.precision}"
        return self.type.value


class Precision:
    _REAL_PRECISIONS = {
        PrecisionType.F32,
        PrecisionType.F64,
        PrecisionType.FLONG,
    }

    _COMPLEX_PRECISIONS = {
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

    @classmethod
    def is_real_precision(cls, precision: PrecisionConfig) -> bool:
        return precision.type in cls._REAL_PRECISIONS

    @classmethod
    def is_complex_precision(cls, precision: PrecisionConfig) -> bool:
        return precision.type in cls._COMPLEX_PRECISIONS

    @classmethod
    def is_arb_precision(cls, precision: PrecisionConfig) -> bool:
        return precision.type == PrecisionType.ARB

    @classmethod
    def zero_for_precision(cls, precision: PrecisionConfig):
        if cls.is_arb_precision(precision):
            arb_precision = precision.precision or 50
            return ps.Arb(0.0, arb_precision)  # Default precision if not specified
        if cls.is_complex_precision(precision):
            real_zero = cast_real_subtype_value(precision, 0)
            constructor = cls._COMPLEX_CLASS[precision.type]
            if precision.type == PrecisionType.CARB:
                # For CArb, create with precision
                arb_precision = precision.precision or 50
                real_arb = ps.Arb(0.0, arb_precision)
                imag_arb = ps.Arb(0.0, arb_precision)
                return constructor(real_arb, imag_arb)
            return constructor(real_zero)
        return 0.0

    @classmethod
    def one_for_precision(cls, precision: PrecisionConfig):
        if cls.is_arb_precision(precision):
            arb_precision = precision.precision or 50
            return ps.Arb(1.0, arb_precision)  # Default precision if not specified
        if cls.is_complex_precision(precision):
            real_one = cast_real_subtype_value(precision, 1)
            constructor = cls._COMPLEX_CLASS[precision.type]
            if precision.type == PrecisionType.CARB:
                # For CArb, create with precision
                arb_precision = precision.precision or 50
                real_arb = ps.Arb(1.0, arb_precision)
                imag_arb = ps.Arb(0.0, arb_precision)
                return constructor(real_arb, imag_arb)
            return constructor(real_one)
        return 1.0

    @classmethod
    def cast_precision_value(cls, precision: PrecisionConfig, value: Any):
        if value is None or isinstance(value, bool):
            return value

        if cls.is_real_precision(precision):
            if isinstance(value, (int, float)):
                return float(value)
            try:
                return float(value)
            except (TypeError, ValueError):
                return value

        if cls.is_arb_precision(precision):
            if isinstance(value, ps.Arb):
                return value
            return ps.Arb(str(value), precision.precision or 50)

        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision.type]
            if isinstance(value, cls):
                return value
            if isinstance(value, str):
                sanitized_str = value.replace(" ", "").replace("i", "j")
                try:
                    value = complex(sanitized_str)
                except ValueError:
                    pass
            if isinstance(value, complex):
                if precision.type == PrecisionType.CARB:
                    real_arb = ps.Arb(str(value.real), precision.precision or 50)
                    imag_arb = ps.Arb(str(value.imag), precision.precision or 50)
                    return constructor(real_arb, imag_arb)
                return constructor(value.real, value.imag)
            if isinstance(value, (tuple, list)) and len(value) == 2:
                if precision.type == PrecisionType.CARB:
                    real_arb = ps.Arb(str(value[0]), precision.precision or 50)
                    imag_arb = ps.Arb(str(value[1]), precision.precision or 50)
                    return constructor(real_arb, imag_arb)
                return constructor(value[0], value[1])
            if precision.type == PrecisionType.CARB and not isinstance(value, ps.Arb):
                return constructor(ps.Arb(str(value), precision.precision or 50))
            return constructor(value)

        return value

    @classmethod
    def cast_real_subtype_value(cls, precision: PrecisionConfig, value: Any):
        real_precision_type = cls._REAL_SUBTYPE_FOR_PRECISION.get(
            precision.type, precision.type
        )
        real_precision = PrecisionConfig(real_precision_type, precision.precision)
        return cls.cast_precision_value(real_precision, value)

    @classmethod
    def series_result_ctor_for_precision(cls, precision: PrecisionConfig):
        return cls._SERIES_RESULT_CLASS[precision.type]

    @classmethod
    def cast_natural_series_value(cls, precision: PrecisionConfig, value: str):
        if cls.is_real_precision(precision):
            return float(value)
        if cls.is_arb_precision(precision):
            return ps.Arb(value, precision.precision or 50)
        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision.type]
            real_value = cast_real_subtype_value(precision, value)
            try:
                return constructor(real_value)
            except TypeError:
                if precision.type == PrecisionType.CARB:
                    return constructor(ps.Arb(value, precision.precision or 50))
                return constructor(value)
        return value

    @classmethod
    def create_series_result(cls, values: list[Any], precision: PrecisionConfig):
        cumulative = []
        total = cls.zero_for_precision(precision)
        for v in values:
            total = total + v
            cumulative.append(total)
        ctor = cls.series_result_ctor_for_precision(precision)
        return ctor(cumulative, values)


cast_real_subtype_value = Precision.cast_real_subtype_value
zero_for_precision = Precision.zero_for_precision
series_result_ctor_for_precision = Precision.series_result_ctor_for_precision
cast_precision_value = Precision.cast_precision_value
cast_natural_series_value = Precision.cast_natural_series_value
is_arb_precision = Precision.is_arb_precision
create_series_result = Precision.create_series_result
