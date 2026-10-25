"""
CSV series parameter source implementation.
Author: Yadrentsev I. M.
"""

import csv
from pathlib import Path
from typing import Iterable

from shanks.runner.domain.params import SeriesParamCSV
from shanks.runner.domain.precision import PrecisionType
from shanks.runner.domain.sources import SeriesParamSource
from shanks.runner.config.model import NoiseConfig


class CSVSeriesParamSource(SeriesParamSource):
    def __init__(self, path: Path):
        self.path = path

    def load(self, precision: PrecisionType) -> Iterable[SeriesParamCSV]:
        """Loads series parameters from a CSV file."""
        results = []
        with open(self.path, encoding="utf-8") as f:
            for i, row in enumerate(csv.reader(f), start=1):
                results.append(
                    SeriesParamCSV(
                        precision=precision,
                        location=self.path,
                        row=i,
                        raw_values=tuple(row),
                    )
                )
        return results

    def load_noises(self) -> list[NoiseConfig]:
        """Loads noise configurations from provided data.

        :return: A list of NoiseConfig instances.
        :rtype: list[NoiseConfig]
        """
        return []
