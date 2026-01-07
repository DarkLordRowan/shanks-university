"""
Parquet export service with split tables for series and acceleration results.
Author: Yadrentsev I. M.
"""

from pathlib import Path
from typing import Sequence, Any

import pyarrow as pa
import pyarrow.parquet as pq
import pyshanks as ps

from src.domain.export_service import ExportService
from src.domain.precision import PrecisionType


class ParquetSplitExportService(ExportService):

    @staticmethod
    def sanitize_complex_value(value: Any) -> dict[str, Any] | None:
        """Sanitize complex-like value for Parquet export.

        Sanitize complex-like value for Parquet export.
        Adds 'real' and 'imag' keys for complex numbers, and handles other numeric types.

        :param value: The value to sanitize.
        :type value: Any
        :raises ValueError: If the value is not a recognized type.
        :return: The sanitized value.
        :rtype: dict[str, Any] | None
        """
        if value is None:
            return None
        elif isinstance(value, (ps.CF32, ps.CF64, ps.CFLong, ps.CArb)):
            return {"real": str(value.real), "imag": str(value.imag)}
        elif isinstance(value, complex):
            return {"real": str(value.real), "imag": str(value.imag)}
        elif isinstance(value, (ps.Arb, float, int)):
            return {"real": str(value), "imag": None}
        else:
            raise ValueError(value)

    @staticmethod
    def sanitize_value(value: Any) -> Any:
        """Sanitize a value for Parquet export.

        Sanitizes a value by converting complex types to structured representations
        or recursively processing dictionaries and lists.

        Sanitization rules:
        - pyshanks numeric types (Arb, CArb, CF32, CF64, CFLong, float, int, complex) are converted to structured dicts.
        - pyshanks RemainderType and NumeratorType are converted to their name strings.
        - PrecisionType enum values are converted to their string values.
        - dictionaries and lists are recursively sanitized.
        - strings are returned as-is.
        - None values are returned as None.

        :param value: The value to sanitize.
        :type value: Any
        :raises ValueError: If the value is not a recognized type.
        :return: The sanitized value.
        :rtype: Any
        """
        if isinstance(
            value,
            (
                ps.Arb,
                ps.CArb,
                ps.CF32,
                ps.CF64,
                ps.CFLong,
                float,
                int,
                complex,
            ),
        ):
            return ParquetSplitExportService.sanitize_complex_value(value)
        elif isinstance(value, (ps.RemainderType, ps.NumeratorType)):
            return value.name
        elif isinstance(value, PrecisionType):
            return value.value
        elif isinstance(value, dict):
            if value:
                return {
                    k: ParquetSplitExportService.sanitize_value(v)
                    for k, v in value.items()
                }
            else:
                return None
        elif isinstance(value, (list, tuple)):
            return [ParquetSplitExportService.sanitize_value(item) for item in value]
        elif isinstance(value, str):
            return str(value)
        elif value is None:
            return None
        else:
            raise ValueError(value)

    @staticmethod
    def build_series_rows(series_params):
        """Builds rows for series Parquet table."""
        rows = []
        for sp in series_params:
            for args, (
                series_result,
                lim,
            ) in sp.pregen.items():
                arg_dict = {k: str(v) for k, v in args}

                rows.append(
                    {
                        "series_name": sp.name,
                        "series_id": sp.id,
                        "precision": sp.precision.value,
                        "arguments": {
                            "x": arg_dict.get("x"),
                            "b": arg_dict.get("b"),
                            "m": arg_dict.get("m"),
                            "a": arg_dict.get("a"),
                        },
                        "series_limit": {
                            "real": str(lim),
                            "imag": None,
                        },
                        "computed": [
                            ParquetSplitExportService.sanitize_complex_value(sn)
                            for sn in series_result.Sn
                        ],
                    }
                )
        return rows

    @staticmethod
    def build_accel_rows(dicts):
        """Builds rows for acceleration Parquet table."""
        rows = []

        for item in dicts:
            accel = item["accel"]
            series = item["series"]

            computed_list = []
            for c in item["computed"]:
                computed_list.append(
                    {
                        "value": {
                            "real": str(c["accel_value"]),
                            "imag": None,
                        },
                        "deviation": str(c["accel_value_deviation"]),
                    }
                )

            error_list = []
            event_list = []
            for c in item["computed"]:
                for e in c.get("events", []):
                    event_list.append(
                        {
                            "n": c["n"],
                            "name": e["name"],
                            "description": e["description"],
                        }
                    )

            rows.append(
                {
                    "series_id": series["id"],
                    "accel_name": accel["name"],
                    "m_value": accel["m_value"],
                    "additional_args": {
                        "remainder": accel["additional_args"].get("remainder"),
                        "useRecurrentFormula": accel["additional_args"].get(
                            "useRecurrentFormula"
                        ),
                        "beta": None,
                        "gamma": None,
                        "parameter": None,
                        "numerator": None,
                        "rho": None,
                        "epsilon_threshold": None,
                    },
                    "computed": computed_list,
                    "errors": error_list,
                    "events": event_list,
                }
            )

        return rows

    def __init__(self, location: Path):
        self.location = location
        self.series_schema = pa.schema(
            [
                ("series_name", pa.string()),
                ("series_id", pa.int64()),
                ("precision", pa.string()),
                (
                    "arguments",
                    pa.struct(
                        [
                            ("x", pa.string()),
                            ("b", pa.string()),
                            ("m", pa.string()),
                            ("a", pa.string()),
                        ]
                    ),
                ),
                (
                    "series_limit",
                    pa.struct(
                        [
                            ("real", pa.string()),
                            ("imag", pa.string()),
                        ]
                    ),
                ),
                (
                    "computed",
                    pa.list_(
                        pa.struct(
                            [
                                ("real", pa.string()),
                                ("imag", pa.string()),
                            ]
                        ),
                    ),
                ),
            ]
        )
        self.accel_schema = pa.schema(
            [
                ("series_id", pa.int64()),
                ("accel_name", pa.string()),
                ("m_value", pa.int64()),
                (
                    "additional_args",
                    pa.struct(
                        [
                            ("remainder", pa.string()),
                            ("useRecurrentFormula", pa.string()),
                            ("beta", pa.string()),
                            ("gamma", pa.string()),
                            ("parameter", pa.string()),
                            ("numerator", pa.string()),
                            ("rho", pa.string()),
                            ("epsilon_threshold", pa.string()),
                        ]
                    ),
                ),
                (
                    "computed",
                    pa.list_(
                        pa.struct(
                            [
                                (
                                    "value",
                                    pa.struct(
                                        [
                                            ("real", pa.string()),
                                            ("imag", pa.string()),
                                        ]
                                    ),
                                ),
                                ("deviation", pa.string()),
                            ]
                        )
                    ),
                ),
                (
                    "errors",
                    pa.list_(pa.struct([("n", pa.int64()), ("message", pa.string())])),
                ),
                (
                    "events",
                    pa.list_(
                        pa.struct(
                            [
                                ("n", pa.int64()),
                                ("name", pa.string()),
                                ("description", pa.string()),
                            ]
                        )
                    ),
                ),
            ]
        )

    def export(self, dicts: Sequence[dict], **kwargs):
        """Export implementation for Parquet with split tables."""
        series = kwargs.get("series", [])
        series_rows = self.build_series_rows(series)
        accel_rows = self.build_accel_rows(dicts)

        series_table = pa.Table.from_pylist(series_rows, schema=self.series_schema)
        accel_table = pa.Table.from_pylist(accel_rows, schema=self.accel_schema)

        pq.write_table(
            series_table, self.location / "series.parquet", compression="zstd"
        )
        pq.write_table(accel_table, self.location / "accel.parquet", compression="zstd")
