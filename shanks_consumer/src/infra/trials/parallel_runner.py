import multiprocessing as mp

from tqdm import tqdm

from src.domain.trial_result import (AccelTrialResult, ErrorTrialResult,
                                     SeriesTrialResult, TrialResult)
from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class ParallelTrialRunner(TrialRunner):

    def __init__(
        self,
        process_count: int = 1,
        timeout: float = 10,
        memory_efficient: bool = False,
    ):
        self.process_count = process_count
        self.timeout = timeout
        self.memory_efficient = memory_efficient

    def run(self, combinations):
        if self.memory_efficient:
            return self.__run_dispose_at_completion(combinations)
        yield self.__run_full_load(combinations)

    def __run_full_load(self, combinations):
        if not combinations:
            return []

        results = []
        process_count = self.process_count or mp.cpu_count()

        with mp.Pool(processes=process_count) as pool:
            async_tasks = [
                (pool.apply_async(execute_trial, (comb,)), comb)
                for comb in combinations
            ]

            for async_result, comb in tqdm(async_tasks, desc="Trials"):
                try:
                    chunk = async_result.get(timeout=self.timeout)
                    if chunk:
                        results.extend(chunk)
                except mp.TimeoutError:
                    series, accel = comb
                    results.append(
                        TrialResult(
                            SeriesTrialResult(series.series_name, None, {}),
                            AccelTrialResult(accel.accel_name, -1, {}),
                            computed=[],
                            error=ErrorTrialResult(
                                f"Timeout after {self.timeout}s",
                                data={
                                    "series": series.series_name,
                                    "accel": accel.accel_name,
                                },
                            ),
                        )
                    )

        return results

    def __run_dispose_at_completion(self, combinations):
        if not combinations:
            return []

        process_count = self.process_count or mp.cpu_count()

        with mp.Pool(processes=process_count) as pool:
            async_tasks = [
                (pool.apply_async(execute_trial, (comb,)), comb)
                for comb in combinations
            ]

            for async_result, comb in async_tasks:
                try:
                    chunk = async_result.get(timeout=self.timeout)
                    if chunk:
                        yield chunk
                except mp.TimeoutError:
                    series, accel = comb
                    yield [
                        TrialResult(
                            SeriesTrialResult(series.series_name, None, {}),
                            AccelTrialResult(accel.accel_name, -1, {}),
                            computed=[],
                            error=ErrorTrialResult(
                                f"Timeout after {self.timeout}s",
                                {
                                    "series": series.series_name,
                                    "accel": accel.accel_name,
                                },
                            ),
                        )
                    ]
