from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class SequentialTrialRunner(TrialRunner):

    def run(self, combinations, pregen_series=None):
        return (execute_trial(combo, pregen_series) for combo in combinations)
