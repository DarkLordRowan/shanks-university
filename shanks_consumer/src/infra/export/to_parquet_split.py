from pathlib import Path
from typing import Sequence

import pyarrow as pa
import pyarrow.parquet as pq

from src.domain.export_service import ExportService


class ParquetSplitExportService(ExportService):
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
                                ("n", pa.int64()),
                                (
                                    "value",
                                    pa.struct(
                                        [
                                            ("real", pa.string()),
                                            ("imag", pa.string()),
                                        ]
                                    ),
                                ),
                            ]
                        )
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
                    pa.list_(
                        pa.struct(
                            [("n", pa.int64()), ("message", pa.string())]
                        )
                    ),
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
        if not dicts:
            table = pa.table({})
        else:
            table = pa.Table.from_pylist(dicts)

        pq.write_table(
            table,
            self.location / "split.parquet",
            compression="zstd",
            write_statistics=True,
        )
