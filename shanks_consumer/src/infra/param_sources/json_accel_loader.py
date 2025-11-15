import json
from pathlib import Path
from typing import Iterable

from src.domain.params import AccelParamJSON
from src.domain.precision import PrecisionType
from src.infra.param_sources.data_accel_loader import DataAccelParamSource
from src.infra.param_sources.decoder import decide_json_decoder


class JSONAccelParamSource(DataAccelParamSource):
    def __init__(self, path: Path):
        self.path = path
        super().__init__({})

    def load(self, precision: PrecisionType) -> Iterable[AccelParamJSON]:
        decoder = decide_json_decoder(precision)
        self.data = json.loads(self.path.read_text(), cls=decoder)

        return super().load(precision=precision)
