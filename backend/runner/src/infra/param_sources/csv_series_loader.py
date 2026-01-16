"""
CSV series parameter source implementation.
Author: Yadrentsev I. M.
"""

import csv
from pathlib import Path
from typing import Iterable

from src.domain.params import SeriesParamCSV
from src.domain.precision import PrecisionType
from src.domain.sources import SeriesParamSource
from src.config.model import NoiseConfig


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
