from src.domain.trial_runner import TrialRunner
from src.domain.use_cases.run_trial import execute_trial


class SequentialTrialRunner(TrialRunner):

    def run(self, combinations):
        results = []
        for combo in combinations:
            results.extend(execute_trial(combo))
        return results
