import logging
import uuid

from src.config import TrialConfig
from src.db import setup_mongo_db
from src.run.export import ExportTrialResults
from src.run.loaders import AccelParamLoader, SeriesParamLoader
from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.trial import ComplexTrial, SeriesRecord, AccelRecord


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
) -> tuple[list[SeriesRecord], list[AccelRecord]]:
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
    series_records, accel_records = trial.execute()

    return series_records, accel_records


def export_results(
    series_records: list[SeriesRecord],
    accel_records: list[AccelRecord],
    config: TrialConfig,
):
    logging.info("Exporting results...")

    results_exporter = ExportTrialResults(
        series_records, accel_records
    )
    
    for format_type in config.output_formats:
        if format_type.value == "parquet":
            logging.info("Exporting to Parquet...")
            results_exporter.to_parquet(config.output_dir, config.results_filename)
            logging.info("Results exported to Parquet")
        elif format_type.value == "json":
            logging.info("Exporting to JSON...")
            json_path = config.output_dir / f"{config.results_filename}.json"
            results_exporter.to_json(json_path)
            logging.info("Results exported to JSON")
            
            # Also export visualization-compatible JSON
            viz_json_path = config.output_dir / f"{config.results_filename}_viz.json"
            results_exporter.to_viz_json(viz_json_path)
            logging.info("Results exported to visualization JSON")
        else:
            logging.warning(f"Export format {format_type.value} not supported in new schema")


def handle_run_command(config: TrialConfig):
    aggregated_series_records: list[SeriesRecord] = []
    aggregated_accel_records: list[AccelRecord] = []
    stack_id = str(uuid.uuid4())
    
    for precision in config.precisions:
        series_records, accel_records = execute_trial(config, precision, stack_id)
        aggregated_series_records.extend(series_records)
        aggregated_accel_records.extend(accel_records)

    export_results(aggregated_series_records, aggregated_accel_records, config)
    logging.info(
        "Results are obtainable via stack_id: [%s]",
        stack_id,
    )
