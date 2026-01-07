"""
Handles the run command for executing trials.

Author: Yadrentsev I. M.
"""

from src.config.model import TrialConfig
from src.dependency import get_trial_executor_from_config


def handle_run_command(config: TrialConfig):
    """
    Handles the run command by executing all precisions for the given trial configuration.

    Parameters:
    config (TrialConfig): The configuration for the trial.

    Returns:
    None

    Author: Yadrentsev I. M.
    """
    executor = get_trial_executor_from_config(config)
    executor.run_all_precisions()
