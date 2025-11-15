import logging

from src.config import TrialConfig
from src.dependency import get_trial_executor_from_config


def handle_run_command(config: TrialConfig):
    logging.info("Starting trials...")

    executor = get_trial_executor_from_config(config)
    stack_id = executor.run_all_precisions()

    logging.info("Results ready, stack_id: %s", stack_id)
