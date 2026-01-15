"""
Precision handling for numerical series and acceleration methods.
Author: Shevyrov A.N., Yadrentsev I. M.
"""

from typing import Any, Protocol, Sequence, TypeVar, runtime_checkable

import pyshanks as ps


@runtime_checkable
class NumericLike(Protocol):
    """Protocol for numeric-like types supporting basic arithmetic operations."""

    def __add__(self, other: Any) -> Any: ...
    def __sub__(self, other: Any) -> Any: ...
    def __abs__(self) -> Any: ...


TNum = TypeVar("TNum", bound=NumericLike)


class SeriesResultProto(Protocol[TNum]):
    """Protocol for series result representation."""

    Sn: Sequence[TNum]
    an: Sequence[TNum]


class SeriesBaseProto(Protocol[TNum]):
    """Protocol for series generation and summation."""

    def generate(self, n: int) -> SeriesResultProto[TNum]: ...

    def get_sum(self) -> TNum: ...


class AccelProto(Protocol[TNum]):
    """Protocol for acceleration methods."""

    def __call__(self, n: int, m: int, series: SeriesResultProto[TNum]) -> TNum: ...


class PrecisionType:
    """Supported precision types for numerical computations.
    
    Handles both standard types (F32, F64, etc.) and arbitrary precision types
    with explicit bit counts (e.g., "Arb256", "CArb128").
    """
    
    def __init__(self, name: str):
        self.name = name
        self.suffix = name
        self.bits: int | None = None
        
        # Parse dynamic arbitrary precision types
        if name.startswith("Arb"):
            self.suffix = "Arb"
            if len(name) > 3 and name[3:].isdigit():
                self.bits = int(name[3:])
        elif name.startswith("CArb"):
            self.suffix = "CArb"
            if len(name) > 4 and name[4:].isdigit():
                self.bits = int(name[4:])
                
        # Handle standard types (ensure suffix is correct if name matches)
        if name in ["F32", "F64", "FLong", "CF32", "CF64", "CFLong"]:
            self.suffix = name

    @property
    def value(self) -> str:
        """Returns the backend suffix (e.g., 'Arb' for 'Arb256')."""
        return self.suffix

    def __str__(self) -> str:
        return self.name
        
    def __repr__(self) -> str:
        return f"PrecisionType('{self.name}')"
        
    def __eq__(self, other: Any) -> bool:
        if isinstance(other, PrecisionType):
            return self.name == other.name
        if isinstance(other, str):
            return self.name == other
        return False
        
    def __hash__(self) -> int:
        return hash(self.name)

# Define standard constants for backward compatibility and convenience
PrecisionType.F32 = PrecisionType("F32")
PrecisionType.F64 = PrecisionType("F64")
PrecisionType.FLONG = PrecisionType("FLong")
PrecisionType.ARB = PrecisionType("Arb")  # Deprecated generic Arb
PrecisionType.CF32 = PrecisionType("CF32")
PrecisionType.CF64 = PrecisionType("CF64")
PrecisionType.CFLONG = PrecisionType("CFLong")
PrecisionType.CARB = PrecisionType("CArb") # Deprecated generic CArb


class Precision:
    """Precision specific utilities for numerical computations.

    Provides methods for handling different numerical precisions, including real and complex types.
    """

    _REAL_SUFFIXES = {
        "F32", "F64", "FLong", "Arb"
    }

    _COMPLEX_SUFFIXES = {
        "CF32", "CF64", "CFLong", "CArb"
    }

    _COMPLEX_CLASS = {
        "CF32": complex,
        "CF64": complex,
        "CFLong": ps.CFLong,
        "CArb": ps.CArb,
    }

    _SERIES_RESULT_CLASS = {
        "F32": ps.SeriesResultF32,
        "F64": ps.SeriesResultF64,
        "FLong": ps.SeriesResultFLong,
        "Arb": ps.SeriesResultArb,
        "CF32": ps.SeriesResultCF32,
        "CF64": ps.SeriesResultCF64,
        "CFLong": ps.SeriesResultCFLong,
        "CArb": ps.SeriesResultCArb,
    }

    _REAL_SUBTYPE_SUFFIX = {
        "CF32": "F32",
        "CF64": "F64",
        "CFLong": "FLong",
        "CArb": "Arb",
    }

    @classmethod
    def is_real_precision(cls, precision: PrecisionType) -> bool:
        """Check if the given precision is a real number precision."""
        return precision.suffix in cls._REAL_SUFFIXES

    @classmethod
    def is_complex_precision(cls, precision: PrecisionType) -> bool:
        """Check if the given precision is a complex number precision."""
        return precision.suffix in cls._COMPLEX_SUFFIXES

    @classmethod
    def is_arb_precision(cls, precision: PrecisionType) -> bool:
        """Check if the given precision is arbitrary precision."""
        return precision.suffix == "Arb"

    @classmethod
    def zero_for_precision(cls, precision: PrecisionType):
        """Get the zero value for the specified precision."""
        if cls.is_arb_precision(precision):
            # Use explicit precision if available, otherwise default (53)
            prec = precision.bits if precision.bits else 53
            return ps.Arb(0.0, prec)
            
        if cls.is_complex_precision(precision):
            real_zero = cast_real_subtype_value(precision, 0)
            constructor = cls._COMPLEX_CLASS[precision.suffix]
            return constructor(real_zero)
        return 0.0

    @classmethod
    def cast_precision_value(cls, precision: PrecisionType, value: Any):
        """Cast the given value to the specified precision type."""
        if value is None or isinstance(value, bool):
            return value

        if cls.is_real_precision(precision):
            if precision.suffix == "Arb":
                # Handle Arbitrary Precision
                if isinstance(value, ps.Arb):
                    return value
                
                # Use explicit precision from config (e.g. 256 from Arb256)
                # Fallback to 53 if not specified (legacy behavior)
                prec = precision.bits if precision.bits else 53
                
                if isinstance(value, (int, float)):
                    return ps.Arb(float(value), prec)
                return ps.Arb(str(value), prec)
            
            # Handle Standard Real Types
            if isinstance(value, (int, float)):
                return float(value)
            try:
                return float(value)
            except (TypeError, ValueError):
                return value

        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision.suffix]
            
            if isinstance(value, complex):
                if precision.suffix == "CArb":
                     # Decompose and reconstruct with high precision
                     real_part = cls.cast_real_subtype_value(precision, value.real)
                     imag_part = cls.cast_real_subtype_value(precision, value.imag)
                     return constructor(real_part, imag_part)
                return constructor(value.real, value.imag)
                
            if isinstance(value, (tuple, list)) and len(value) == 2:
                # Recursively cast components (handles CArb logic via cast_real_subtype_value)
                real_part = cls.cast_real_subtype_value(precision, value[0])
                imag_part = cls.cast_real_subtype_value(precision, value[1])
                return constructor(real_part, imag_part)
                
            if precision.suffix == "CArb" and not isinstance(value, ps.Arb):
                # Construct from single value (real part)
                real_part = cls.cast_real_subtype_value(precision, value)
                return constructor(real_part)
                
            return constructor(value)

        return value

    @classmethod
    def cast_real_subtype_value(cls, precision: PrecisionType, value: Any):
        """Cast the given value to the real subtype of the specified precision."""
        # We need to construct a PrecisionType for the real subtype
        # If we have CArb256, we want Arb256.
        # If we have CF64, we want F64.
        
        real_suffix = cls._REAL_SUBTYPE_SUFFIX.get(precision.suffix, precision.suffix)
        
        # If it's dynamic (Arb/CArb), preserve bits
        real_name = real_suffix
        if precision.bits:
             # Reconstruct name: Arb + 256
             real_name = f"{real_suffix}{precision.bits}"
             
        real_precision = PrecisionType(real_name)
        return cls.cast_precision_value(real_precision, value)

    @classmethod
    def series_result_ctor_for_precision(cls, precision: PrecisionType):
        """Get the series result constructor for the specified precision."""
        return cls._SERIES_RESULT_CLASS[precision.suffix]

    @classmethod
    def cast_natural_series_value(cls, precision: PrecisionType, value: str):
        """Cast a natural number string to the specified precision type."""
        if cls.is_real_precision(precision):
            if precision.suffix == "Arb":
                 prec = precision.bits if precision.bits else 53
                 return ps.Arb(value, prec)
            return float(value)
            
        if cls.is_complex_precision(precision):
            constructor = cls._COMPLEX_CLASS[precision.suffix]
            real_value = cast_real_subtype_value(precision, value)
            try:
                return constructor(real_value)
            except TypeError:
                # Fallback for complex types constructed from single argument
                return constructor(value)
        return value

    @classmethod
    def create_series_result(cls, values: list[Any], precision: PrecisionType):
        """Create a series result for the specified precision from a list of values."""
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