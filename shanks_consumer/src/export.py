from src.trial import TrialResult
from typing import Iterable
import pathlib
import json
import csv
from dataclasses import asdict


class ExportTrialResult:
    def __init__(
        self,
        results: Iterable[TrialResult],
        location: pathlib.Path | None = None,
    ):
        self.results = results
        self.location = location

    def _verify_location(
        self, override_location: pathlib.Path | None
    ) -> pathlib.Path:
        location = override_location or self.location
        if not location:
            raise ValueError("Provide location to export")
        return location

    def to_json(self, override_location: pathlib.Path | None = None):
        with open(
            self._verify_location(override_location),
            mode="w",
            encoding="utf-8",
        ) as f:
            f.write(
                json.dumps(
                    [asdict(result) for result in self.results],
                    indent=4,
                    sort_keys=True,
                )
            )

    def to_csv(self, override_location: pathlib.Path):
        with open(
            self._verify_location(override_location),
            mode="w",
            encoding="utf-8",
        ) as f:
            csv_writer = csv.writer(f)
            for result in self.results:
                result_context = (
                    list(map(str, asdict(result.series).values()))
                    + list(map(str, asdict(result.accel).values()))
                    + (
                        list(map(str, asdict(result.error).values()))
                        if result.error
                        else []
                    )
                )
                for compute in result.computed:
                    csv_writer.writerow(
                        result_context
                        + list(map(str, asdict(compute).values()))
                    )
