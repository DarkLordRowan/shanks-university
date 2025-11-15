import logging
import uuid
from typing import Sequence

from src.config import TrialConfig
from src.domain.complex_trial import ComplexTrial
from src.domain.export_service import ExportService
from src.domain.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.domain.sources import AccelParamSource, SeriesParamSource
from src.domain.trial_result import TrialResult
from src.domain.trial_runner import TrialRunner


class TrialExecutor:
    def __init__(
        self,
        config: TrialConfig,
        runner: TrialRunner,
        series_sources: Sequence[SeriesParamSource],
        accel_sources: Sequence[AccelParamSource],
        exporters: Sequence[ExportService],
    ):
        self.config = config
        self.runner = runner
        self.stack_id: str = str(uuid.uuid4())

        self.series_sources = series_sources
        self.accel_sources = accel_sources
        self.exporters = exporters

    def load_parameters(self, precision: PrecisionType):
        series_params: list[BaseSeriesParam] = []
        accel_params: list[BaseAccelParam] = []

        for source in self.series_sources:
            series_params.extend(source.load(precision))

        for source in self.accel_sources:
            accel_params.extend(source.load(precision))

        if not series_params:
            raise ValueError("No series parameters found!")

        logging.info(
            "Loaded %d series params for precision %s",
            len(series_params),
            precision.name,
        )
        logging.info(
            "Loaded %d accel params for precision %s",
            len(accel_params),
            precision.name,
        )

        return series_params, accel_params

    def run_trials(self, precision: PrecisionType) -> list[TrialResult]:
        logging.info("Running trials for precision: %s", precision.name)
        series_params, accel_params = self.load_parameters(precision)

        trial = ComplexTrial(
            series_params,
            accel_params,
            stack_id=self.stack_id,
        )

        results = self.runner.run(trial.combinations())

        if not self.config.no_events:
            results = [res.load_events() for res in results]

        return results

    def export_results(self, results: list[TrialResult]):
        logging.info("Exporting results...")
        for exporter in self.exporters:
            exporter.export(results, config=self.config)

    def run_all_precisions(self) -> str:
        aggregated: list[TrialResult] = []

        for precision in self.config.precisions:
            logging.info("Running precision: %s", precision.name)
            res = self.run_trials(precision)
            aggregated.extend(res)

        self.export_results(aggregated)
        return self.stack_id
