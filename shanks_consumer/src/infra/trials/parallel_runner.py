import multiprocessing as mp

from tqdm import tqdm

from src.domain.trial_result import (
    AccelTrialResult,
    ErrorTrialResult,
    SeriesTrialResult,
    TrialResult,
)
from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class ParallelTrialRunner(TrialRunner):

    def __init__(self, process_count=1, timeout=10):
        self.process_count = process_count
        self.timeout = timeout

    def run(self, combinations):
        if not combinations:
            return []

        results = []
        process_count = self.process_count or mp.cpu_count()

        with mp.Pool(processes=process_count) as pool:
            async_tasks = [
                (pool.apply_async(execute_trial, (comb,)), comb)
                for comb in combinations
            ]

            for async_result, comb in tqdm(async_tasks, desc="Trials", ncols=100):
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
                                {
                                    "series": series.series_name,
                                    "accel": accel.accel_name,
                                },
                            ),
                        )
                    )

        return results
