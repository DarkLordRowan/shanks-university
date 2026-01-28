"""
Precision handling for numerical series and acceleration methods.
Author: Shevyrov A.N., Yadrentsev I. M.
"""

from typing import Any, Protocol, Sequence, TypeVar, runtime_checkable

import pyshanks as ps


# --- Protocols ---

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


# --- Precision Definition ---

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

# Define standard constants
PrecisionType.F32 = PrecisionType("F32")
PrecisionType.F64 = PrecisionType("F64")
PrecisionType.FLONG = PrecisionType("FLong")
PrecisionType.ARB = PrecisionType("Arb")
PrecisionType.CF32 = PrecisionType("CF32")
PrecisionType.CF64 = PrecisionType("CF64")
PrecisionType.CFLONG = PrecisionType("CFLong")
PrecisionType.CARB = PrecisionType("CArb")


# --- Helper Functions ---

def _get_complex_components(value: Any) -> tuple[Any, Any]:
    """Decompose a value into real and imaginary parts."""
    if isinstance(value, complex):
        return value.real, value.imag
    if isinstance(value, (tuple, list)) and len(value) == 2:
        return value[0], value[1]
    if hasattr(value, "real") and hasattr(value, "imag"):
        return value.real, value.imag
    return value, 0.0


def _get_arb_precision(precision: PrecisionType) -> int:
    """Get the bit precision for Arb types, defaulting to 53."""
    return precision.bits if precision.bits else 53


# --- Core Conversion Logic ---

def cast_precision_value(precision: PrecisionType, value: Any) -> Any:
    """Convert a value to the target precision type suitable for computation.
    
    This function handles the crucial boundary between raw configuration values
    and strict C++ bindings (pyshanks).
    """
    if value is None or isinstance(value, bool):
        return value

    suffix = precision.suffix

    # --- Arbitrary Precision ---
    if suffix == "Arb":
        bits = _get_arb_precision(precision)
        if isinstance(value, ps.Arb):
            return value # Assume bits are compatible
        return ps.Arb(value, bits)

    if suffix == "CArb":
        bits = _get_arb_precision(precision)
        if isinstance(value, ps.CArb):
            return value
        r, i = _get_complex_components(value)
        # Must construct components as Arb first
        return ps.CArb(ps.Arb(r, bits), ps.Arb(i, bits))

    # --- Extended Precision (Long Double) ---
    if suffix == "FLong":
        if isinstance(value, ps.FLong):
            return value
        return ps.FLong(float(value))

    if suffix == "CFLong":
        if isinstance(value, ps.CFLong):
            return value
        r, i = _get_complex_components(value)
        # Must construct components as FLong first
        return ps.CFLong(ps.FLong(float(r)), ps.FLong(float(i)))

    # --- Standard Precision ---
    # F32/F64 are mapped to Python float for simplicity and JSON compatibility,
    # relying on implicit conversion in bindings where necessary.
    if suffix in ("F32", "F64"):
        return float(value)

    if suffix in ("CF32", "CF64"):
        if isinstance(value, complex):
            return value
        r, i = _get_complex_components(value)
        return complex(float(r), float(i))

    return value


def cast_natural_series_value(precision: PrecisionType, value: str) -> Any:
    """Cast a string (typically from CSV) to the precision type."""
    if precision.suffix == "Arb":
        # Arb constructor handles strings directly to preserve precision
        return cast_precision_value(precision, value)
    
    # For others, parse as float first
    try:
        float_val = float(value)
    except ValueError:
        float_val = 0.0
        
    return cast_precision_value(precision, float_val)


def cast_real_subtype_value(precision: PrecisionType, value: Any) -> Any:
    """Cast value to the real subtype of the given precision.
    
    E.g. if precision is CF32, cast to F32.
    If precision is CArb256, cast to Arb256.
    """
    suffix = precision.suffix
    
    # Determine target real suffix
    target_suffix = suffix
    if suffix.startswith("C") and suffix not in ("CSVSeries",): # Basic check
        target_suffix = suffix[1:]
        
    # Reconstruct name for PrecisionType
    real_name = target_suffix
    if precision.bits:
        real_name = f"{target_suffix}{precision.bits}"
        
    real_prec = PrecisionType(real_name)
    return cast_precision_value(real_prec, value)


def create_series_result(values: list[Any], precision: PrecisionType) -> Any:
    """Create a SeriesResult object populated with values."""
    suffix = precision.suffix
    cls_name = f"SeriesResult{suffix}"
    
    if not hasattr(ps, cls_name):
         raise ValueError(f"Unknown series result type: {cls_name}")
    
    ctor = getattr(ps, cls_name)
    
    # Calculate cumulative sum with correct zero type
    cumulative = []
    total = cast_precision_value(precision, 0)
    
    for v in values:
        total = total + v
        cumulative.append(total)
        
    return ctor(cumulative, values)
