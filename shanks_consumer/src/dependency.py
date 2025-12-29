from src.config.model import TrialConfig
from src.infra.export.registry import EXPORT_BUILDERS
from src.infra.export.serializer import TrialResultSerializer
from src.infra.param_sources.registry import (PARAM_ACCELS_SOURCE_BUILDERS,
                                              PARAM_SERIES_SOURCE_BUILDERS)
from src.infra.trials.parallel_runner import ParallelTrialRunner
from src.infra.trials.sequential_runner import SequentialTrialRunner
from src.logger import setup_logging
from src.services.trial_executor import TrialExecutor


def get_trial_executor_from_config(config: TrialConfig) -> TrialExecutor:
    logger = setup_logging(config.verbose)
    logger.debug(config)
    runner = (
        ParallelTrialRunner(
            process_count=config.trial_process_count,
            timeout=config.trial_task_timeout,
            memory_efficient=config.trial_memory_efficient,
        )
        if config.is_parallel
        else SequentialTrialRunner()
    )

    series_sources = [build(config) for build in PARAM_SERIES_SOURCE_BUILDERS]
    if not all(series_sources):
        raise ValueError(f"Failed collecting series: {series_sources}")

    accel_sources = [build(config) for build in PARAM_ACCELS_SOURCE_BUILDERS]
    if not all(accel_sources):
        raise ValueError(f"Failed collecting accels: {accel_sources}")

    serializer = TrialResultSerializer(view=config.trial_result_view)

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
