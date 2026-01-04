"""
Sequential trial runner implementation.
Author: Sobolev Y. A.
"""

from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class SequentialTrialRunner(TrialRunner):
    """Simple sequential trial runner."""

    def run(self, combinations):
        return (execute_trial(combo) for combo in combinations)
