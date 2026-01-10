"""
Sequential trial runner implementation.
Author: Sobolev Y. A.
"""

from collections.abc import Iterable

from src.config.model import TrialConfig
from src.domain.params import BaseAccelParam, BaseSeriesParam
from src.domain.trial_result import TrialResult
from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class SequentialTrialRunner(TrialRunner):
    """Simple sequential trial runner."""

    def __init__(self, config: TrialConfig):
        self.config = config

    def run(
        self,
        combinations: Iterable[tuple[BaseSeriesParam, BaseAccelParam]],
    ) -> list[TrialResult]:
        results = []

        for series, accel in combinations:
            # Run as-is
            results.extend(execute_trial((series, accel), noise_config=None))

            # Run with noise
            for noise_config in self.config.noise_configs:
                results.extend(
                    execute_trial((series, accel), noise_config=noise_config)
                )

        return results
