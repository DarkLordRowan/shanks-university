import io
from typing import Iterable
import pathlib
import json
import csv
from dataclasses import asdict, fields
from pyshanks import FP

from src.trial import (
    TrialResult,
    AccelTrialResult,
    ErrorTrialResult,
    SeriesTrialResult,
    ComputedTrialResult,
)

from src.events import TrialEvent


def dataclass_fields_with_prefix(dataclass_type, prefix: str) -> list[str]:
    return list(map(lambda s: prefix + s.name, fields(dataclass_type)))

class FPEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, FP):
            return str(o)
        return super().default(o)

def dataclasses_to_json(dataclasses, location):
    for i in dataclasses:
        print(i)
    with open(
        location,
        mode="w",
        encoding="utf-8",
    ) as f:
        f.write(
            json.dumps(
                [asdict(dataclass) for dataclass in dataclasses],
                indent=4,
                sort_keys=True,
                cls=FPEncoder
            )
        )


class BaseExport:
    def __init__(self, location: pathlib.Path | None = None):
        self.location = location

    def _verify_location(
        self, override_location: pathlib.Path | None
    ) -> pathlib.Path:
        location = override_location or self.location
        if not location:
            raise ValueError("Provide location to export")
        return location


class ExportTrialResults(BaseExport):
    def __init__(
        self,
        results: Iterable[TrialResult],
        location: pathlib.Path | None = None,
    ):
        self.results = results
        super().__init__(location)

    def to_json(self, override_location: pathlib.Path | None = None):
        dataclasses_to_json(
            self.results, self._verify_location(override_location)
        )

    def as_dict(self) -> list[dict]:
        return [asdict(result) for result in self.results]

    def to_csv(self, override_location: pathlib.Path):
        with open(
            self._verify_location(override_location),
            mode="w",
            encoding="utf-8",
        ) as f:
            csv_writer = csv.writer(f)
            csv_writer.writerow(
                dataclass_fields_with_prefix(SeriesTrialResult, "series_")
                + dataclass_fields_with_prefix(AccelTrialResult, "accel_")
                + dataclass_fields_with_prefix(ErrorTrialResult, "error_")
                + dataclass_fields_with_prefix(
                    ComputedTrialResult, "computed_"
                )
            )
            for result in self.results:
                result_context = (
                    list(map(str, asdict(result.series).values()))
                    + list(map(str, asdict(result.accel).values()))
                    + (
                        list(map(str, asdict(result.error).values()))
                        if result.error
                        else list(
                            map(str, asdict(ErrorTrialResult("", {})).values())
                        )
                    )
                )
                for compute in result.computed:
                    csv_writer.writerow(
                        result_context
                        + list(map(str, asdict(compute).values()))
                    )

    def to_csv_text(self) -> str:
        buf = io.StringIO()
        writer = csv.writer(buf)
        writer.writerow(
            dataclass_fields_with_prefix(SeriesTrialResult, "series_")
            + dataclass_fields_with_prefix(AccelTrialResult, "accel_")
            + dataclass_fields_with_prefix(ErrorTrialResult, "error_")
            + dataclass_fields_with_prefix(ComputedTrialResult, "computed_")
        )
        for result in self.results:
            context = (
                    list(map(str, asdict(result.series).values()))
                    + list(map(str, asdict(result.accel).values()))
                    + (
                        list(map(str, asdict(result.error).values()))
                        if result.error
                        else list(map(str, asdict(ErrorTrialResult("", {})).values()))
                    )
            )
            for compute in result.computed:
                writer.writerow(context + list(map(str, asdict(compute).values())))
        return buf.getvalue()

    def to_csv_bytes(self, encoding: str = "utf-8") -> bytes:
        return self.to_csv_text().encode(encoding)


class ExportTrialEvents(BaseExport):
    def __init__(
        self,
        events: Iterable[TrialEvent],
        location: pathlib.Path | None = None,
    ):
        self.events = events
        super().__init__(location)

    def to_json(self, override_location: pathlib.Path | None = None):
        dataclasses_to_json(
            self.events, self._verify_location(override_location)
        )

    def as_dict(self) -> list[dict]:
        return [asdict(event) for event in self.events]

    def to_csv(self, override_location: pathlib.Path):
        with open(
            self._verify_location(override_location),
            mode="w",
            encoding="utf-8",
        ) as f:
            csv_writer = csv.writer(f)
            csv_writer.writerow(
                ["event_name", "event_data"]
                + dataclass_fields_with_prefix(SeriesTrialResult, "series_")
                + dataclass_fields_with_prefix(AccelTrialResult, "accel_")
                + dataclass_fields_with_prefix(ErrorTrialResult, "error_")
            )
            for event in self.events:
                csv_writer.writerow(
                    [event.event, event.data]
                    + list(map(str, asdict(event.result.series).values()))
                    + list(map(str, asdict(event.result.accel).values()))
                    + (
                        list(map(str, asdict(event.result.error).values()))
                        if event.result.error
                        else list(
                            map(str, asdict(ErrorTrialResult("", {})).values())
                        )
                    )
                )

    def to_csv_text(self) -> str:
        buf = io.StringIO()
        writer = csv.writer(buf)
        writer.writerow(
            ["event_name", "event_data"]
            + dataclass_fields_with_prefix(SeriesTrialResult, "series_")
            + dataclass_fields_with_prefix(AccelTrialResult, "accel_")
            + dataclass_fields_with_prefix(ErrorTrialResult, "error_")
        )
        for event in self.events:
            writer.writerow(
                [event.event, event.data]
                + list(map(str, asdict(event.result.series).values()))
                + list(map(str, asdict(event.result.accel).values()))
                + (
                    list(map(str, asdict(event.result.error).values()))
                    if event.result.error
                    else list(map(str, asdict(ErrorTrialResult("", {})).values()))
                )
            )
        return buf.getvalue()

    def to_csv_bytes(self, encoding: str = "utf-8") -> bytes:
        return self.to_csv_text().encode(encoding)
