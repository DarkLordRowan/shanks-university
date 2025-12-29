import itertools
import uuid
from dataclasses import dataclass, field

from src.domain.params import BaseAccelParam, BaseSeriesParam


@dataclass
class ComplexTrial:
    series_params: list[BaseSeriesParam]
    accel_params: list[BaseAccelParam]
    stack_id: str = field(default_factory=lambda: str(uuid.uuid4()))

    def combinations(self) -> list[tuple[BaseSeriesParam, BaseAccelParam]]:
        return list(itertools.product(self.series_params, self.accel_params))
