"""
Parallel trial runner implementation.
Author: Sobolev Y. A.
"""

import multiprocessing as mp
from collections.abc import Iterable

from src.config.model import TrialConfig, NoiseConfig
from src.domain.params import BaseAccelParam, BaseSeriesParam
from src.domain.trial_result import (
    AccelTrialResult,
    ErrorTrialResult,
    SeriesTrialResult,
    TrialResult,
)
from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class ParallelTrialRunner(TrialRunner):
    """Parallel trial runner using multiprocessing.

    Implements the TrialRunner protocol to execute trials in parallel using multiple processes.
    """

    def __init__(self, config: TrialConfig):
        self.config = config

    def run(
        self,
        combinations: Iterable[tuple[BaseSeriesParam, BaseAccelParam]],
    ) -> list[TrialResult]:
        tasks = []
        for series, accel in combinations:
            # Run as-is
            tasks.append((series, accel, None, self.config.filter_configs))

            # Run with noise
            for noise_config in self.config.noise_configs:
                tasks.append((series, accel, noise_config, self.config.filter_configs))

        if self.config.trial_memory_efficient:
            return self.__run_dispose_at_completion(tasks)
        return self.__run_full_load(tasks)

    def __run_full_load(self, tasks):
        """Full load trial execution.

        Executes all trials and collects results before returning.
        """
        if not tasks:
            return []

        results = []
        process_count = self.config.trial_process_count or mp.cpu_count()

        with mp.Pool(processes=process_count) as pool:
            async_tasks = [
                (
                    pool.apply_async(execute_trial, ((series, accel), noise, filters)),
                    (series, accel, noise),
                )
                for series, accel, noise, filters in tasks
            ]

            for async_result, (series, accel, noise) in async_tasks:
                try:
                    chunk = async_result.get(timeout=self.config.trial_task_timeout)
                    if chunk:
                        results.extend(chunk)
                except mp.TimeoutError:
                    results.append(
                        TrialResult(
                            SeriesTrialResult(
                                series.id, series.series_name, None, {}
                            ),
                            AccelTrialResult(accel.accel_name, -1, {}),
                            computed=[],
                            noise=noise,
                            error=ErrorTrialResult(
                                f"Timeout after {self.config.trial_task_timeout}s",
                                data={
                                    "series": series.series_name,
                                    "accel": accel.accel_name,
                                },
                            ),
                        )
                    )

        return results

    def __run_dispose_at_completion(self, tasks):
        """Memory-efficient trial execution.

        Executes trials and yields results as they complete to minimize memory usage.
        """
        if not tasks:
            return []

        process_count = self.config.trial_process_count or mp.cpu_count()

        with mp.Pool(
            processes=process_count,
        ) as pool:
            async_tasks = [
                (
                    pool.apply_async(execute_trial, ((series, accel), noise, filters)),
                    (series, accel, noise),
                )
                for series, accel, noise, filters in tasks
            ]

            for async_result, (series, accel, noise) in async_tasks:
                try:
                    chunk = async_result.get(timeout=self.config.trial_task_timeout)
                    if chunk:
                        yield chunk
                except mp.TimeoutError:
                    yield [
                        TrialResult(
                            SeriesTrialResult(
                                series.id, series.series_name, None, {}
                            ),
                            AccelTrialResult(accel.accel_name, -1, {}),
                            computed=[],
                            noise=noise,
                            error=ErrorTrialResult(
                                f"Timeout after {self.config.trial_task_timeout}s",
                                data={
                                    "series": series.series_name,
                                    "accel": accel.accel_name,
                                },
                            ),
                        )
                    ]
