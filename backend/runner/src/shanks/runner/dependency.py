"""
Dependency injection for trial executor.
Author: Yadrentsev I. M.
"""

from shanks.runner.config.model import TrialConfig
from shanks.runner.infra.export.registry import EXPORT_BUILDERS
from shanks.runner.infra.export.serializer import TrialResultSerializer
from shanks.runner.infra.param_sources.registry import (
    PARAM_ACCELS_SOURCE_BUILDERS,
    PARAM_SERIES_SOURCE_BUILDERS,
)
from shanks.runner.infra.trials.parallel_runner import ParallelTrialRunner
from shanks.runner.logger import setup_logging
from shanks.runner.services.trial_executor import TrialExecutor


def get_trial_executor_from_config(config: TrialConfig) -> TrialExecutor:
    """Gets a TrialExecutor instance based on the provided configuration.

    It sets up the necessary components such as the trial runner, parameter sources,
    serializer, and exporters according to the configuration.

    :param config: Trial configuration
    :type config: TrialConfig
    :raises ValueError: If parameter sources cannot be collected
    :raises ValueError: If exporters cannot be collected
    :return: Configured TrialExecutor instance
    :rtype: TrialExecutor
    """
    logger = setup_logging(config.verbose)
    logger.debug(config)
    runner = ParallelTrialRunner(config=config)

    series_sources = [build(config) for build in PARAM_SERIES_SOURCE_BUILDERS]
    if not all(series_sources):
        raise ValueError(f"Failed collecting series: {series_sources}")

    accel_sources = [build(config) for build in PARAM_ACCELS_SOURCE_BUILDERS]
    if not all(accel_sources):
        raise ValueError(f"Failed collecting accels: {accel_sources}")

    serializer = TrialResultSerializer()

    exporters = [
        exporter
        for fmt in config.output_formats
        if fmt in EXPORT_BUILDERS
        for exporter in [EXPORT_BUILDERS[fmt](config)]
        if exporter is not None
    ]
    return TrialExecutor(
        config=config,
        runner=runner,
        series_sources=series_sources,
        accel_sources=accel_sources,  # type: ignore
        serializer=serializer,
        exporters=exporters,
        logger=logger,
    )
