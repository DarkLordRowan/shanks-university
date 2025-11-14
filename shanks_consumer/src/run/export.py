import csv
import io
import json
import pathlib
from collections.abc import Mapping, Sequence
from dataclasses import Field, asdict, fields, is_dataclass
from typing import Any, cast

from pymongo.database import Database as MongoDatabase
from tqdm import tqdm

import pyshanks as ps
from src.run.params import PrecisionType
from src.run.trial import TrialResult


def auto_field_prefix(outer_field: Field, prefix: str = "", separator: str = "_"):
    return (
        f"{prefix}{outer_field.name}{separator}"
        if prefix
        else f"{outer_field.name}{separator}"
    )


def flatten_dataclass(
    obj: Any, prefix: str = "", separator: str = "_"
) -> dict[str, Any]:
    if not is_dataclass(obj):
        return {}

    result = {}
    for _field in fields(obj):
        field_value = getattr(obj, _field.name)

        if is_dataclass(field_value):
            nested = flatten_dataclass(
                field_value,
                auto_field_prefix(_field, prefix, separator),
                separator,
            )
            result.update(nested)
        else:
            key = prefix + _field.name if prefix else _field.name
            result[key] = field_value

    return result


def get_flattened_headers(
    dataclass_type: Any,
    prefix: str = "",
    separator: str = "_",
    exclude_fields: list[str] | None = None,
):
    if not is_dataclass(dataclass_type):
        return [prefix.rstrip(separator)] if prefix else []

    exclude_fields = exclude_fields or []

    headers = []
    for _field in fields(dataclass_type):
        if _field.name in exclude_fields:
            continue

        field_type = _field.type
        field_prefix = auto_field_prefix(_field, prefix, separator)

        if is_dataclass(field_type):
            nested_headers = get_flattened_headers(field_type, field_prefix, separator)
            headers.extend(nested_headers)
        else:
            headers.append(field_prefix.rstrip(separator))

    return headers


# pyright: reportAttributeAccessIssue=false
def get_expanded_field_headers(dataclass_type, field_name, separator="_"):
    if not is_dataclass(dataclass_type):
        return []

    for _field in fields(dataclass_type):
        if _field.name == field_name:
            field_type = _field.type
            if (
                hasattr(field_type, "__origin__")
                and field_type.__origin__ is list
                and getattr(field_type, "__args__", None)
                and is_dataclass(field_type.__args__[0])
            ):
                nested_type = field_type.__args__[0]
                return get_flattened_headers(
                    nested_type, prefix=f"{field_name}_", separator=separator
                )
    return []


def _write_dataclasses_to_csv_writer(dataclasses, writer, expand_field, separator):
    if not dataclasses:
        return

    for dataclass_obj in tqdm(dataclasses, desc="Exporting to CSV"):
        if expand_field:
            expand_data = getattr(dataclass_obj, expand_field)
            if not isinstance(expand_data, list):
                raise ValueError(f"Field '{expand_field}' must be a list")

            base_data = {}
            for _field in fields(dataclass_obj):
                if _field.name != expand_field:
                    field_value = getattr(dataclass_obj, _field.name)
                    if is_dataclass(field_value):
                        flattened_field = flatten_dataclass(
                            field_value,
                            prefix=f"{_field.name}_",
                            separator=separator,
                        )
                        base_data.update(flattened_field)
                    else:
                        base_data[_field.name] = field_value

            for item in expand_data:
                item_flattened = flatten_dataclass(
                    item, prefix=f"{expand_field}_", separator=separator
                )
                row_data = {**base_data, **item_flattened}
                final_row = {
                    header: row_data.get(header, "") for header in writer.fieldnames
                }
                writer.writerow(final_row)
        else:
            flattened = flatten_dataclass(dataclass_obj, separator=separator)
            final_row = {
                header: flattened.get(header, "") for header in writer.fieldnames
            }
            writer.writerow(final_row)


def dataclasses_to_csv(dataclasses, location, expand_field=None, separator="_"):
    if not dataclasses:
        return

    first_obj = dataclasses[0]

    if expand_field:
        base_headers = get_flattened_headers(
            type(first_obj), separator=separator, exclude_fields=expand_field
        )
        expanded_headers = get_expanded_field_headers(
            type(first_obj), expand_field, separator=separator
        )
        headers = base_headers + expanded_headers
    else:
        headers = get_flattened_headers(type(first_obj), separator=separator)

    with open(location, mode="w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=headers)
        writer.writeheader()
        _write_dataclasses_to_csv_writer(dataclasses, writer, expand_field, separator)


def dataclasses_to_csv_text(dataclasses, expand_field=None, separator="_"):
    if not dataclasses:
        return ""

    first_obj = dataclasses[0]

    if expand_field:
        base_headers = get_flattened_headers(
            type(first_obj), separator=separator, exclude_fields=expand_field
        )
        expanded_headers = get_expanded_field_headers(
            type(first_obj), expand_field, separator=separator
        )
        headers = base_headers + expanded_headers
    else:
        headers = get_flattened_headers(type(first_obj), separator=separator)

    buf = io.StringIO()
    writer = csv.DictWriter(buf, fieldnames=headers)
    writer.writeheader()
    _write_dataclasses_to_csv_writer(dataclasses, writer, expand_field, separator)

    return buf.getvalue()


class BaseExport:
    def __init__(
        self, data: Sequence[TrialResult], location: pathlib.Path | None = None
    ):
        self.location = location
        self.data: list[TrialResult] = list(data)

        self.expand_field: str | None = None
        self.separator: str = "_"
        self.mongodb_collection: str = "base"
        self.batch_size = 1000

    # TODO: Unfortunately, this is still used to sanitize `error`. Better clean this up.
    @staticmethod
    def _sanitize_value(value: Any, *, convert_precision: bool = False) -> Any:
        if isinstance(
            value,
            (ps.Arb, ps.CArb, ps.CF32, ps.CF64, ps.CFLong, float),
        ):
            return str(value)
        if isinstance(
            value,
            (
                ps.RemainderType,
                ps.NumeratorType,
            ),
        ):
            return value.name
        if convert_precision and isinstance(value, PrecisionType):
            return value.value

        if is_dataclass(value):
            return BaseExport._sanitize_value(
                asdict(cast(Any, value)), convert_precision=convert_precision
            )

        if isinstance(value, Mapping):
            return {
                key: BaseExport._sanitize_value(
                    val, convert_precision=convert_precision
                )
                for key, val in value.items()
            }

        if isinstance(value, Sequence) and not isinstance(
            value, (str, bytes, bytearray)
        ):
            return [
                BaseExport._sanitize_value(item, convert_precision=convert_precision)
                for item in value
            ]

        return value

    def _to_dict_fast(self) -> list[dict[str, Any]]:
        """Fast conversion to dict format using direct field access for TrialResult."""
        result_dicts = []

        for result in tqdm(self.data, desc="Converting results"):
            # Build nested structure like original but with direct field access
            result_dict = {
                "series": {
                    "name": result.series.name,
                    "arguments": {
                        k: str(v) for k, v in result.series.arguments.items()
                    },
                    "lim": str(result.series.lim),
                },
                "accel": {
                    "name": result.accel.name,
                    "m_value": str(result.accel.m_value),
                    "additional_args": {
                        k: str(v) for k, v in result.accel.additional_args.items()
                    },
                },
                "computed": [
                    {
                        "n": computed.n,
                        "series_value": str(computed.series_value),
                        "partial_sum": str(computed.partial_sum),
                        "partial_sum_deviation": str(computed.partial_sum_deviation),
                        "accel_value": str(computed.accel_value),
                        "accel_value_deviation": str(computed.accel_value_deviation),
                    }
                    for computed in result.computed
                ],
                "error": {
                    "description": result.error.description,
                    "data": self._sanitize_value(result.error.data)
                    if result.error
                    else None,
                }
                if result.error
                else None,
                "stack_id": result.stack_id,
            }

            result_dicts.append(result_dict)

        return result_dicts

    def as_dict(self) -> list[dict[str, Any]]:
        serialized: list[dict[str, Any]] = []
        for dataclass_obj in self.data:
            if not is_dataclass(dataclass_obj):
                raise TypeError("BaseExport expects dataclass instances")
            serialized.append(
                cast(
                    dict[str, Any],
                    self._sanitize_value(
                        asdict(cast(Any, dataclass_obj)), convert_precision=True
                    ),
                )
            )
        return serialized

    def _verify_location(self, override_location):
        location = override_location or self.location
        if not location:
            raise ValueError("Provide location to export")
        return location

    def to_parquet(self, output_dir: pathlib.Path, filename: str):
        import pandas as pd

        parquet_file = output_dir / f"{filename}.parquet"
        # Ensure directory exists
        output_dir.mkdir(parents=True, exist_ok=True)

        # Create DataFrame and save to parquet
        df = pd.DataFrame(self._to_dict_fast())
        df.to_parquet(
            parquet_file,
            engine="pyarrow",
            compression="zstd",
            index=False,
            write_statistics=True,
        )

    def to_mongodb(self, mongo_database: MongoDatabase):
        data_dicts = self.as_dict()
        collection = mongo_database.get_collection(self.mongodb_collection)

        with tqdm(
            total=len(data_dicts),
            desc=f"Exporting to MongoDB collection '{self.mongodb_collection}'",
        ) as pbar:
            for i in range(0, len(data_dicts), self.batch_size):
                batch = data_dicts[i : i + self.batch_size]
                collection.insert_many(batch)
                pbar.update(len(batch))

    def to_json(self, override_location: pathlib.Path | None = None):
        location = self._verify_location(override_location)

        with open(location, mode="w", encoding="utf-8") as f:
            json.dump(self._to_dict_fast(), f, indent=4, sort_keys=True)

    def to_csv(
        self,
        override_location: pathlib.Path | None = None,
    ):
        dataclasses_to_csv(
            self.data,
            self._verify_location(override_location),
            expand_field=self.expand_field,
            separator=self.separator,
        )

    def to_csv_text(self):
        return dataclasses_to_csv_text(
            self.data, expand_field=self.expand_field, separator=self.separator
        )

    def to_csv_bytes(self, encoding: str = "utf-8"):
        return self.to_csv_text().encode(encoding)


class ExportTrialResults(BaseExport):
    def __init__(
        self,
        results: list[TrialResult],
        collection_name: str = "trial_results",
        location: pathlib.Path | None = None,
    ):
        super().__init__(results, location)
        self.expand_field = "computed"
        self.collection_name = collection_name
