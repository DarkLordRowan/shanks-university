"""
JSON acceleration parameter source implementation.
Author: Yadrentsev I. M.
"""

import json
from pathlib import Path
from typing import Iterable

from src.domain.params import AccelParamJSON
from src.domain.precision import PrecisionType
from src.infra.param_sources.data_accel_loader import DataAccelParamSource


class JSONAccelParamSource(DataAccelParamSource):
    def __init__(self, path: Path):
        self.path = path
        super().__init__({})

    def load(self, precision: PrecisionType) -> Iterable[AccelParamJSON]:
        """Loads acceleration parameters from a JSON file.

        :param precision: The precision type for parameter conversion.
        :type precision: PrecisionType
        :return: An iterable of AccelParamJSON instances.
        :rtype: Iterable[AccelParamJSON]
        """
        self.data = json.loads(self.path.read_text())

        return super().load(precision=precision)
