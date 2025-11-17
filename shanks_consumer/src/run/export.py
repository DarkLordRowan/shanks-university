import json
import pathlib
from dataclasses import asdict, fields, is_dataclass
from typing import Any, Sequence

import pyarrow as pa
import pyarrow.dataset as ds
from tqdm import tqdm

import pyshanks as ps
from src.run.params import PrecisionType
from src.run.trial import (
    AccelPoint,
    AccelRecord,
    ErrorRecord,
    EventRecord,
    SeriesRecord,
)


def sanitize_complex_value(value: Any) -> dict[str, str | None] | None:
    """Convert complex numbers to {real: str, imag: str} format."""
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


def sanitize_value(value: Any) -> Any:
    """Sanitize values for Parquet export."""
    if isinstance(
        value, (ps.Arb, ps.CArb, ps.CF32, ps.CF64, ps.CFLong, float, int, complex)
    ):
        return sanitize_complex_value(value)
    elif isinstance(value, (ps.RemainderType, ps.NumeratorType)):
        return value.name
    elif isinstance(value, PrecisionType):
        return value.value
    elif isinstance(value, dict):
        if value:
            return {k: sanitize_value(v) for k, v in value.items()}
        else:
            return None
    elif isinstance(value, (list, tuple)):
        return [sanitize_value(item) for item in value]
    elif isinstance(value, str):
        return str(value)
    elif value is None:
        return None
    else:
        raise ValueError(value)


def sanitize_series_record(record: SeriesRecord) -> dict[str, Any]:
    """Sanitize a SeriesRecord for Parquet export."""
    sanitized = {
        "series_name": record.series_name,
        "series_id": record.series_id,
        "precision": record.precision.value
        if isinstance(record.precision, PrecisionType)
        else str(record.precision),
        "arguments": record.source_arguments,
        "series_limit": sanitize_complex_value(record.series_limit),
        "computed": [],
    }

    # Sanitize computed points
    for point in record.computed:
        sanitized_point = {"n": point.n, "value": sanitize_complex_value(point.value)}
        sanitized["computed"].append(sanitized_point)

    return sanitized


def sanitize_accel_record(record: AccelRecord) -> dict[str, Any]:
    """Sanitize an AccelRecord for Parquet export."""
    sanitized = {
        "series_id": record.series_id,
        "accel_name": record.accel_name,
        "m_value": record.m_value,
        "additional_args": record.source_additional_args,
        "computed": [],
        "errors": None,
        "events": None,
    }

    # Sanitize computed values
    for point in record.computed:
        if point is None:
            sanitized["computed"].append(None)
        else:
            sanitized_point = {
                "value": sanitize_complex_value(point.value),
                "deviation": sanitize_complex_value(point.deviation),
            }
            sanitized["computed"].append(sanitized_point)

    # Sanitize errors if present
    if record.errors:
        sanitized["errors"] = [
            {"message": str(error.message), "n": error.n} for error in record.errors
        ]

    # Sanitize events if present
    if record.events:
        sanitized["events"] = [
            {
                "name": str(event.name),
                "n": event.n,
                "description": str(event.description),
            }
            for event in record.events
        ]

    return sanitized


class ExportTrialResults:
    def __init__(
        self,
        series_records: list[SeriesRecord],
        accel_records: list[AccelRecord],
        output_dir: pathlib.Path | None = None,
    ):
        self.series_records = series_records
        self.accel_records = accel_records
        self.output_dir = output_dir

    def to_parquet(self, output_dir: pathlib.Path, filename: str):
        """Export to separate Parquet files for series and accelerations."""
        output_dir.mkdir(parents=True, exist_ok=True)
        print("exporting")

        # Export series records
        if self.series_records:
            series_data = []
            for record in tqdm(self.series_records, desc="Sanitizing series records"):
                series_data.append(sanitize_series_record(record))

            series_table = pa.Table.from_pylist(series_data)
            series_path = output_dir / "parquet" / "series"

            ds.write_dataset(
                series_table,
                base_dir=series_path,
                format="parquet",
                partitioning=ds.partitioning(
                    pa.schema(
                        [
                            ("series_name", pa.string()),
                        ]
                    ),
                    flavor="hive",
                ),
                use_threads = False, # 20.0.0 deadlocks otherwise
                existing_data_behavior="overwrite_or_ignore",
            )
            print(f"Series data exported to: {series_path}")
        print("accels now")

        # Export acceleration records
        if self.accel_records:
            accel_data = []
            for record in tqdm(
                self.accel_records, desc="Sanitizing acceleration records"
            ):
                accel_data.append(sanitize_accel_record(record))

            # TODO: Remove or make non-hardcoded
            accel_schema = pa.schema(
                [
                    ("series_id", pa.int64()),
                    ("accel_name", pa.string()),
                    ("m_value", pa.int64()),
                    (
                        "additional_args",
                        pa.struct(
                            [
                                ("remainder", pa.string()),
                                ("useRecFormulas", pa.string()),
                                ("beta", pa.string()),
                                ("gamma", pa.string()),
                                ("parameter", pa.string()),
                                ("numerator", pa.string()),
                                ("RHO", pa.string()),
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
                                    (
                                        "deviation",
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
                    (
                        "errors",
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
                                    (
                                        "error",
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
                    (
                        "events",
                        pa.list_(
                            pa.struct(
                                [
                                    ("n", pa.int64()),
                                    ("event_type", pa.string()),
                                    ("description", pa.string()),
                                ]
                            )
                        ),
                    ),
                ]
            )

            print("Creating table with explicit schema...")
            accel_table = pa.Table.from_pylist(accel_data, schema=accel_schema)
            accel_path = output_dir / "parquet" / "accelerations"

            ds.write_dataset(
                accel_table,
                base_dir=accel_path,
                format="parquet",
                partitioning=ds.partitioning(
                    pa.schema(
                        [
                            ("series_id", pa.int64()),
                        ]
                    ),
                    flavor="hive",
                ),
                existing_data_behavior="overwrite_or_ignore",
                # Disable parallel writing to avoid executor deadlock
                use_threads=False,
                # Conservative file handle limit
                max_open_files=128,
                # Prevent excessive partition fragmentation
                max_partitions=2048,
                # Ensure reasonable file sizes
                max_rows_per_file=10_000_000,
                min_rows_per_group=100_000,
            )
            print(f"Acceleration data exported to: {accel_path}")
        print("done")

    def to_json(self, output_path: pathlib.Path):
        """Export to a single JSON file containing all data."""
        output_path.parent.mkdir(parents=True, exist_ok=True)

        # Combine all data into a single structure
        export_data = {"series": [], "accelerations": []}

        # Export series records
        if self.series_records:
            for record in tqdm(
                self.series_records, desc="Sanitizing series records for JSON"
            ):
                export_data["series"].append(sanitize_series_record(record))

        # Export acceleration records
        if self.accel_records:
            for record in tqdm(
                self.accel_records, desc="Sanitizing acceleration records for JSON"
            ):
                export_data["accelerations"].append(sanitize_accel_record(record))

        # Write to JSON file
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(export_data, f, indent=2, ensure_ascii=False)

        print(f"Data exported to JSON: {output_path}")
