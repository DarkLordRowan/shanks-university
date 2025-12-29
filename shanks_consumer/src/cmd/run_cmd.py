from src.config.model import TrialConfig
from src.dependency import get_trial_executor_from_config


def handle_run_command(config: TrialConfig):
    executor = get_trial_executor_from_config(config)
    executor.run_all_precisions()
