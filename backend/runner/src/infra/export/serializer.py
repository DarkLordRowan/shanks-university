"""
TrialResult serializer implementation.
Author: Yadrentsev I. M.
"""

import re
from dataclasses import asdict, is_dataclass
from typing import Any, Sequence

import pyshanks as ps
from src.domain.data_serializer import DataSerializer
from src.domain.trial_result import (
    AccelTrialResult,
    ComputedTrialResult,
    FilteredResults,
    FilterMethodResult,
    SeriesTrialResult,
    TrialResult,
)


class TrialResultSerializer(DataSerializer):
    def __init__(self):
        pass

    def _beautify_numeric(self, value: Any) -> str:
        """Provides a clean string representation for numeric values."""
        if value is None:
            return "None"

        s = str(value)

        # Heuristic to detect floating point noise (e.g., ...000000000001 or ...999999999998)
        # We look for at least 10 identical digits followed by any digits at the end.
        # This typically happens when a decimal like 0.1 is represented in binary.
        match_s = s
        if "e" in s.lower():
            parts = re.split(pattern="[eE]", string=s, maxsplit=1)
            match_s = parts[0]

        if re.search(r"([0-9])\1{10,}\d*$", match_s):
            try:
                # If we detect noise, Python's str(float(...)) is excellent at finding
                # the shortest equivalent decimal representation for 64-bit floats.
                f = float(s)
                s_f = str(f)

                # Only use it if it's actually shorter (don't mess with already short strings)
                if len(s_f) < len(s):
                    return s_f
            except (ValueError, TypeError, OverflowError):
                pass

        # Normalize simple float strings: remove trailing zeros and dot
        if "e" not in s.lower() and "." in s:
            try:
                s = s.rstrip("0").rstrip(".")
            except Exception:
                pass

        return s

    def _sanitize_numeric(self, value: Any, as_struct: bool = False) -> Any:
        if value is None:
            return None

        # Standard pyshanks and python complex types
        if isinstance(value, (ps.CArb, ps.CF32, ps.CF64, ps.CFLong, complex)):
            if as_struct:
                return {
                    "real": self._beautify_numeric(value.real),
                    "imag": self._beautify_numeric(value.imag),
                }
            else:
                return str(value)

        if as_struct:
            return {"real": self._beautify_numeric(value), "imag": "0.0"}

        return self._beautify_numeric(value)

    def _normalize_arg(self, value: Any) -> str:
        # Unbox complex types if imaginary part is exactly zero
        if hasattr(value, "imag") and hasattr(value, "real"):
            try:
                imag_str = str(value.imag).strip().lower()
                # Check if it represents zero (0, 0.0, -0.0) without precision loss
                # Handle scientific notation like 0.000e+00
                base_part = imag_str.split("e")[0]
                is_zero = (
                    base_part.replace("0", "").replace(".", "").replace("-", "") == ""
                )
                if is_zero:
                    value = value.real
            except Exception:
                pass

        # For "mere args", we are much more aggressive with truncation.
        # We prefer a clean float representation if it's shorter than the high-precision one.
        s = str(value)
        try:
            f = float(s)
            s_f = str(f)
            if len(s_f) < len(s):
                return s_f
        except (ValueError, TypeError, OverflowError):
            pass

        return self._beautify_numeric(value)

    def _sanitize_arguments(self, args: dict[str, Any]) -> dict[str, str]:
        return {k: self._normalize_arg(v) for k, v in args.items()}

    def _sanitize_value(self, value: Any) -> Any:
        """Sanitizes a value for serialization.

        Sanitizes a value by converting complex types to string representations
        or recursively processing dataclasses, dictionaries, and lists.

        THe following types are specifically handled:
        - pyshanks numeric types (Arb, CArb, CF32, CF64, CFLong, float) are converted to strings.
        - pyshanks RemainderType and NumeratorType are converted to their name strings.
        - dataclasses are converted to dictionaries with sanitized values.
        - dictionaries and lists are recursively sanitized.

        :param value: The value to sanitize.
        :type value: Any
        :return: The sanitized value.
        :rtype: Any
        """
        if isinstance(value, TrialResult):
            return {
                "series": self._sanitize_value(value.series),
                "accel": self._sanitize_value(value.accel),
                "computed": [self._sanitize_value(c) for c in value.computed],
                "noise": self._sanitize_value(value.noise),
                "error": self._sanitize_value(value.error),
                "stack_id": value.stack_id,
                "filtered": self._sanitize_value(value.filtered),
            }

        if isinstance(value, ComputedTrialResult):
            return {
                "n": value.n,
                "series_value": self._sanitize_numeric(
                    value.series_value, as_struct=True
                ),
                "partial_sum": self._sanitize_numeric(
                    value.partial_sum, as_struct=True
                ),
                "partial_sum_deviation": self._sanitize_numeric(
                    value.partial_sum_deviation, as_struct=False
                ),
                "accel_value": self._sanitize_numeric(
                    value.accel_value, as_struct=True
                ),
                "accel_value_deviation": self._sanitize_numeric(
                    value.accel_value_deviation, as_struct=False
                ),
                "accel_error": self._sanitize_numeric(
                    value.accel_error, as_struct=True
                ),
                "profiling": self._sanitize_value(value.profiling),
                "events": self._sanitize_value(value.events),
            }

        if isinstance(value, SeriesTrialResult):
            return {
                "id": value.id,
                "name": value.name,
                "lim": self._sanitize_numeric(value.lim, as_struct=True),
                "arguments": self._sanitize_arguments(value.arguments),
            }

        if isinstance(value, AccelTrialResult):
            return {
                "name": value.name,
                "m_value": value.m_value,
                "additional_args": self._sanitize_arguments(value.additional_args),
            }

        if isinstance(value, FilteredResults):
            return {
                "start_n": value.start_n,
                "segment_length": value.segment_length,
                "methods": {
                    k: self._sanitize_value(v) for k, v in value.methods.items()
                },
            }

        if isinstance(value, FilterMethodResult):
            return {
                "values": [
                    self._sanitize_numeric(v, as_struct=True) for v in value.values
                ],
                "average": self._sanitize_numeric(value.average, as_struct=True),
            }

        if isinstance(value, (ps.CArb, ps.CF32, ps.CF64, ps.CFLong, complex)):
            return {"real": str(value.real), "imag": str(value.imag)}
        if isinstance(value, (ps.Arb, ps.FLong, ps.F32, ps.F64, float, int)):
            return str(value)
        if isinstance(value, (ps.RemainderType, ps.NumeratorType)):
            return value.name
        if is_dataclass(value):
            return {
                k: self._sanitize_value(v)
                for k, v in asdict(value).items()  # type: ignore
            }
        if isinstance(value, dict):
            return {k: self._sanitize_value(v) for k, v in value.items()}
        if isinstance(value, list):
            return [self._sanitize_value(v) for v in value]
        return value

    def to_dict(
        self,
        results: Sequence[TrialResult],
    ) -> Sequence[dict]:
        return [self._sanitize_value(r) for r in results]
