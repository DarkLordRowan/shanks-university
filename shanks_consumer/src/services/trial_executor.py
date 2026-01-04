"""
Trial executor service.
Author: Shevyrov A.N., Yadrentsev I.M.
"""

import uuid
from logging import Logger
from typing import Sequence

from tqdm import tqdm

from src.config.model import TrialConfig
from src.domain.complex_trial import ComplexTrial
from src.domain.data_serializer import DataSerializer
from src.domain.export_service import ExportService
from src.domain.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.domain.sources import AccelParamSource, SeriesParamSource
from src.domain.trial_runner import TrialRunner


class TrialExecutor:

    def __init__(
        self,
        config: TrialConfig,
        runner: TrialRunner,
        series_sources: Sequence[SeriesParamSource],
        accel_sources: Sequence[AccelParamSource],
        serializer: DataSerializer,
        exporters: Sequence[ExportService],
        logger: Logger,
    ):
        """TrialExecutor initializes the trial execution service.

        It coordinates loading parameters, running trials, serializing results, and exporting them.

        :param config: Configuration for the trial
        :type config: TrialConfig
        :param runner: The trial runner to execute trials
        :type runner: TrialRunner
        :param series_sources: Sources for series parameters
        :type series_sources: Sequence[SeriesParamSource]
        :param accel_sources: Sources for acceleration parameters
        :type accel_sources: Sequence[AccelParamSource]
        :param serializer: Serializer for data
        :type serializer: DataSerializer
        :param exporters: Services to export results
        :type exporters: Sequence[ExportService]
        :param logger: Logger for logging information
        :type logger: Logger
        """
        self.config = config
        self.runner = runner
        self.stack_id: str = str(uuid.uuid4())

        self.series_sources = series_sources
        self.accel_sources = accel_sources
        self.serializer = serializer
        self.exporters = exporters

        self.logger = logger

    def load_parameters(self, precision: PrecisionType):
        """Load parameters for the trial.

        :param precision: Precision type for loading parameters
        :type precision: PrecisionType
        :raises ValueError: If no series parameters are found
        :return: Series and acceleration parameters
        :rtype: tuple[list[BaseSeriesParam], list[BaseAccelParam]]
        """
        series_params: list[BaseSeriesParam] = []
        accel_params: list[BaseAccelParam] = []

        for source in self.series_sources:
            series_params.extend(source.load(precision))

        for source in self.accel_sources:
            accel_params.extend(source.load(precision))

        if not series_params:
            raise ValueError("No series parameters found!")

        self.logger.info(
            "Loaded %d series params for precision %s",
            len(series_params),
            precision.name,
        )
        self.logger.info(
            "Loaded %d accel params for precision %s",
            len(accel_params),
            precision.name,
        )

        return series_params, accel_params

    def export_results(self, dicts: Sequence[dict], series: list[BaseSeriesParam]):
        """Exports the results using the configured exporters.

        :param dicts: The list of result dictionaries to export.
        :type dicts: Sequence[dict]
        :param series: The list of series parameters associated with the results.
        :type series: list[BaseSeriesParam]
        """
        for exporter in self.exporters:
            exporter.export(dicts, config=self.config, series=series)

    def __run_trials_full_load(
        self,
        series_params: list[BaseSeriesParam],
        accel_params: list[BaseAccelParam],
    ) -> None:
        """Runs trials with full result loading.

        This means all results are collected in memory before exporting.
        It collects all combinations and processes them in one go, in a scope of currently processed precision.
        Only after all combinations are processed, results are exported in bulk.
        Interruption during execution may lead to loss of all results.

        :param series_params: Series parameters for the trial
        :type series_params: list[BaseSeriesParam]
        :param accel_params: Acceleration parameters for the trial
        :type accel_params: list[BaseAccelParam]
        """
        self.logger.info("Running standard trials")

        trial = ComplexTrial(
            series_params,
            accel_params,
            stack_id=self.stack_id,
        )

        results, combinations = [], trial.combinations()

        for result in tqdm(
            self.runner.run(
                combinations,
            ),
            total=len(combinations),
        ):
            results.extend(result)

        dicts = self.serializer.to_dict(results)

        self.export_results(dicts, series_params)

    def __run_trials_dispose_at_completion(
        self,
        series_params: list[BaseSeriesParam],
        accel_params: list[BaseAccelParam],
    ) -> None:
        """Runs trials in a memory-efficient manner.

        This means results are exported as they are completed to minimize memory usage, instead of collecting all results first.
        It writes out results in chunks as they are produced, which may leave incompleted trials recorded partially, instead of all-or-nothing.
        Creates writing overhead but saves memory.

        :param series_params: Series parameters for the trial
        :type series_params: list[BaseSeriesParam]
        :param accel_params: Acceleration parameters for the trial
        :type accel_params: list[BaseAccelParam]
        """
        self.logger.info("Running memory efficient trials")

        trial = ComplexTrial(
            series_params,
            accel_params,
            stack_id=self.stack_id,
        )

        combinations = trial.combinations()

        for result_chunk in tqdm(
            self.runner.run(combinations),
            total=len(combinations),
        ):
            self.export_results(self.serializer.to_dict(result_chunk), series_params)

    def run_trials(
        self,
        series_params: list[BaseSeriesParam],
        accel_params: list[BaseAccelParam],
    ):
        """Run trials based on the provided series and acceleration parameters.

        :param series_params: _series parameters for the trial
        :type series_params: list[BaseSeriesParam]
        :param accel_params: _acceleration parameters for the trial
        :type accel_params: list[BaseAccelParam]
        :return: _trial results
        :rtype: None
        """
        if self.config.trial_memory_efficient:
            return self.__run_trials_dispose_at_completion(
                series_params,
                accel_params,
            )
        return self.__run_trials_full_load(series_params, accel_params)

    def run_all_precisions(self) -> str:
        """Run trials for all configured precisions."""
        for precision in self.config.precisions:
            self.logger.info("Running trials for precision: %s", precision.name)
            series_params, accel_params = self.load_parameters(precision)
            self.logger.debug("Series params: \n%s", "\n".join(map(str, series_params)))
            self.logger.debug("Accel params: \n%s", "\n".join(map(str, accel_params)))
            self.run_trials(series_params, accel_params)

        return self.stack_id
