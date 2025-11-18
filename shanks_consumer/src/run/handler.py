import logging
import uuid
from typing import Iterable

from src.config import TrialConfig
from src.db import setup_mongo_db
from src.run.export import ExportTrialResults
from src.run.loaders import AccelParamLoader, SeriesParamLoader
from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.trial import AccelRecord, ComplexTrial, SeriesRecord


def load_parameters(config: TrialConfig, precision: PrecisionType):
    series_params: list[BaseSeriesParam] = []
    accel_params: list[BaseAccelParam] = []

    if config.series_json.exists():
        logging.info("Loading series from JSON: %s", config.series_json)
        series_params.extend(SeriesParamLoader.from_json(config.series_json, precision))
    else:
        logging.warning("Series JSON file not found: %s", config.series_json)

    if config.series_csv.exists():
        logging.info("Loading series from CSV: %s", config.series_csv)
        series_params.extend(SeriesParamLoader.from_csv(config.series_csv, precision))
    else:
        logging.warning("Series CSV file not found: %s", config.series_csv)

    if config.accel_json.exists():
        logging.info("Loading acceleration methods from: %s", config.accel_json)
        accel_params.extend(AccelParamLoader.from_json(config.accel_json, precision))
    else:
        logging.warning("Acceleration JSON file not found: %s", config.accel_json)

    if not series_params:
        raise ValueError("No series parameters found!")

    logging.info("Loaded %d series parameters", len(series_params))
    logging.info("Loaded %d acceleration parameters", len(accel_params))

    return series_params, accel_params


def execute_trial(
    config: TrialConfig, precision: PrecisionType, stack_id: str
) -> Iterable[tuple[list[SeriesRecord], list[AccelRecord]]]:
    logging.info("Starting trial execution for precision: %s", precision.name)
    logging.info("Process count: %d", config.trial_process_count)

    series_params, accel_params = load_parameters(config, precision)

    trial = ComplexTrial(
        series_params,
        accel_params,
        precision=precision,
        process_count=config.trial_process_count,
        task_timeout=config.trial_task_timeout,
        stack_id=stack_id,
    )
    yield from trial.execute()


def export_results(
    series_records: list[SeriesRecord],
    accel_records: list[AccelRecord],
    config: TrialConfig,
):
    logging.info("Exporting results to Parquet...")

    results_exporter = ExportTrialResults(series_records, accel_records)
    results_exporter.to_parquet(config.output_dir, config.results_filename)
    logging.info("Results exported to Parquet")


def handle_run_command(config: TrialConfig):
    stack_id = str(uuid.uuid4())

    for precision in config.precisions:
        for series_records, accel_records in execute_trial(config, precision, stack_id):
            export_results(series_records, accel_records, config)

    logging.info(
        "Results are obtainable via stack_id: [%s]",
        stack_id,
    )
