from src.trial import TrialResult
from typing import Iterable
from collections.abc import Callable
from dataclasses import dataclass, asdict


@dataclass
class TrialEvent:
    result_id: str
    event: str
    data: dict


class TrialEventScanner:

    def __init__(self, results: Iterable[TrialResult]):
        self.results = results
        self._scan_methods: dict[str, Callable] = {
            "slow_accel_method": self._slow_accel_method,
            "divergent_accel_method": self._divergent_accel_method,
        }

    def _slow_accel_method(self, result: TrialResult) -> dict | None:
        for compute in result.computed:
            if compute.accel_value_deviation < compute.partial_sum_deviation:
                return asdict(compute)

    def _divergent_accel_method(self, result: TrialResult):
        for i in range(1, len(result.computed)):
            if (
                result.computed[i - 1].accel_value_deviation
                < result.computed[i].accel_value_deviation
            ):
                return asdict(result.computed[i])

    def execute(self) -> list[TrialEvent]:
        events = []
        for result in self.results:
            for name, method in self._scan_methods.items():
                event_data = method(result)
                if event_data:
                    events.append(TrialEvent(result.id, name, event_data))
        return events
