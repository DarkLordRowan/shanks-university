from enum import Enum
from typing import Any, Protocol, Sequence, TypeVar, runtime_checkable

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
    def is_real_precision(cls, precision: PrecisionType) -> bool:
        return precision in cls._REAL_PRECISIONS

    @classmethod
    def is_complex_precision(cls, precision: PrecisionType) -> bool:
        return precision in cls._COMPLEX_PRECISIONS

    @classmethod
    def is_arb_precision(cls, precision: PrecisionType) -> bool:
        return precision == PrecisionType.ARB

    @classmethod
    def zero_for_precision(cls, precision: PrecisionType):
        if cls.is_arb_precision(precision):
            return ps.Arb(0)
        if cls.is_complex_precision(precision):
            real_zero = cast_real_subtype_value(precision, 0)
            constructor = cls._COMPLEX_CLASS[precision]
            return constructor(real_zero)
        return 0.0

    @classmethod
    def one_for_precision(cls, precision: PrecisionType):
        if cls.is_arb_precision(precision):
            return ps.Arb(1)
        if cls.is_complex_precision(precision):
            real_one = cast_real_subtype_value(precision, 1)
            constructor = cls._COMPLEX_CLASS[precision]
            return constructor(real_one)
        return 1.0

    @classmethod
    def cast_precision_value(cls, precision: PrecisionType, value: Any):
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
            return ps.Arb(str(value))

        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision]
            if isinstance(value, cls):
                return value
            if isinstance(value, str):
                sanitized_str = value.replace(" ", "").replace("i", "j")
                try:
                    value = complex(sanitized_str)
                except ValueError:
                    pass
            if isinstance(value, complex):
                if precision == PrecisionType.CARB:
                    return constructor(ps.Arb(value.real), ps.Arb(value.imag))
                return constructor(value.real, value.imag)
            if isinstance(value, (tuple, list)) and len(value) == 2:
                return constructor(value[0], value[1])
            if precision == PrecisionType.CARB and not isinstance(value, ps.Arb):
                return constructor(ps.Arb(str(value)))
            return constructor(value)

        return value

    @classmethod
    def cast_real_subtype_value(cls, precision: PrecisionType, value: Any):
        real_precision = cls._REAL_SUBTYPE_FOR_PRECISION.get(precision, precision)
        return cls.cast_precision_value(real_precision, value)

    @classmethod
    def series_result_ctor_for_precision(cls, precision: PrecisionType):
        return cls._SERIES_RESULT_CLASS[precision]

    @classmethod
    def cast_natural_series_value(cls, precision: PrecisionType, value: str):
        if cls.is_real_precision(precision):
            return float(value)
        if cls.is_arb_precision(precision):
            return ps.Arb(value)
        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision]
            real_value = cast_real_subtype_value(precision, value)
            try:
                return constructor(real_value)
            except TypeError:
                return constructor(value)
        return value

    @classmethod
    def create_series_result(cls, values: list[Any], precision: PrecisionType):
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
