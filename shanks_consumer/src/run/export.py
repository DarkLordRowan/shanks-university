import csv
import io
import json
import pathlib
from dataclasses import Field, asdict, fields, is_dataclass
from typing import Any

from pymongo.database import Database as MongoDatabase
from src.run.params import XArbEncoder as ArbEncoder
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

    for dataclass_obj in dataclasses:
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


def dataclasses_to_json(dataclasses, location):
    with open(location, mode="w", encoding="utf-8") as f:
        f.write(
            json.dumps(
                [asdict(dataclass) for dataclass in dataclasses],
                indent=4,
                sort_keys=True,
                cls=ArbEncoder,
            )
        )


class BaseExport:
    def __init__(self, data: list, location: pathlib.Path | None = None):
        self.location = location
        self.data = data

        self.expand_field: str | None = None
        self.separator: str = "_"
        self.mongodb_collection: str = "base"

    def as_dict(self):
        return [asdict(data) for data in self.data]

    def to_mongodb(self, mongo_database: MongoDatabase):
        mongo_database.get_collection(self.mongodb_collection).insert_many(
            self.as_dict()
        )

    def _verify_location(self, override_location):
        location = override_location or self.location
        if not location:
            raise ValueError("Provide location to export")
        return location

    def to_json(self, override_location: pathlib.Path | None = None):
        dataclasses_to_json(self.data, self._verify_location(override_location))

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
        mongodb_collection: str = "trial_results",
        location: pathlib.Path | None = None,
    ):
        super().__init__(results, location)
        self.expand_field = "computed"
        self.mongodb_collection = mongodb_collection
