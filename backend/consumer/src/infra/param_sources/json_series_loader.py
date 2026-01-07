"""
JSON series parameter source implementation.
Author: Yadrentsev I. M.
"""

import json
from pathlib import Path
from typing import Iterable

from src.domain.params import SeriesParamJSON
from src.domain.precision import PrecisionType
from src.infra.param_sources.data_series_loader import DataSeriesParamSource


class JSONSeriesParamSource(DataSeriesParamSource):
    def __init__(self, path: Path):
        self.path = path
        super().__init__({})

    def load(self, precision: PrecisionType) -> Iterable[SeriesParamJSON]:
        """Loads series parameters from a JSON file.

        :param precision: The precision type for parameter conversion.
        :type precision: PrecisionType
        :return: An iterable of SeriesParamJSON instances.
        :rtype: Iterable[SeriesParamJSON]
        """
        self.data = json.loads(self.path.read_text())
        return super().load(precision=precision)
