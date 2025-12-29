from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial

class SequentialTrialRunner(TrialRunner):

    def run(self, combinations):
        return (execute_trial(combo) for combo in combinations)
