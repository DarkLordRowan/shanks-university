"""
TrialResult serializer implementation.
Author: Yadrentsev I. M.
"""

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

    def _sanitize_numeric(self, value: Any, as_struct: bool = False) -> Any:
        if value is None:
            return None
        if isinstance(value, (ps.CArb, ps.CF32, ps.CF64, ps.CFLong, complex)):
            if as_struct:
                return {"real": str(value.real), "imag": str(value.imag)}
            else:
                return str(value)
        if isinstance(value, (ps.Arb, float, int)):
            if as_struct:
                return {"real": str(value), "imag": "0.0"}
            else:
                return str(value)
        return str(value)

    def _normalize_arg(self, value: Any) -> str:
        # Unbox complex types if imaginary part is exactly zero
        if hasattr(value, "imag") and hasattr(value, "real"):
            try:
                is_zero = str(value.imag) in ("0", "0.0", "-0.0")
                if is_zero:
                    value = value.real
            except Exception:
                pass

        s = str(value)

        # Normalize simple float strings: remove trailing zeros and dot
        # e.g. "1.0" -> "1", "1.500" -> "1.5"
        # Avoid messing with scientific notation for now to be safe
        if "e" not in s and "E" not in s and "." in s:
            try:
                # Check if it's actually a number before stripping
                # (Simple heuristic: look for digits)
                s = s.rstrip("0").rstrip(".")
            except Exception:
                pass

        return s

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
        if isinstance(value, (ps.Arb, float)):
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
