import logging
import uuid

from src.config import TrialConfig
from src.db import setup_mongo_db
from src.run.export import ExportTrialResults
from src.run.loaders import AccelParamLoader, SeriesParamLoader
from src.run.params import BaseAccelParam, BaseSeriesParam, PrecisionType
from src.run.trial import ComplexTrial, TrialResult


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
):
    logging.info("Starting trial execution for precision: %s", precision.name)
    logging.info("Process count: %d", config.trial_process_count)

    series_params, accel_params = load_parameters(config, precision)

    trial = ComplexTrial(
        series_params,
        accel_params,
        process_count=config.trial_process_count,
        task_timeout=config.trial_task_timeout,
        stack_id=stack_id,
    )
    results = trial.execute()

    return results


def export_results(
    results: list,
    config: TrialConfig,
):
    logging.info("Exporting results...")

    results_exporter = ExportTrialResults(
        results, collection_name=config.mongo_collection
    )
    
    # Setup MongoDB if needed
    mongo_database = None
    if any(format_type.value == "mongodb" for format_type in config.output_formats):
        mongo_database = setup_mongo_db(config)
    
    for format_type in config.output_formats:
        if format_type.value == "json":
            results_exporter.to_json(config.results_json)
            logging.info("Results exported to: %s", config.results_json)
        elif format_type.value == "csv":
            results_exporter.to_csv(config.results_csv)
            logging.info("Results exported to: %s", config.results_csv)
        elif format_type.value == "parquet":
            logging.info("Exporting to Parquet...")
            results_exporter.to_parquet(config.output_dir, config.results_filename)
            logging.info("Results exported to Parquet")
        elif format_type.value == "mongodb":
            if mongo_database is not None:
                logging.info("Exporting to MongoDB...")
                results_exporter.to_mongodb(mongo_database)
                logging.info("Results exported to MongoDB")
            else:
                logging.warning("MongoDB export requested but connection failed")


def lazy_load_events(
    config: TrialConfig, results: list[TrialResult]
) -> list[TrialResult]:
    if not config.no_events:
        results = list(map(lambda r: r.load_events(), results))
    return results


def handle_run_command(config: TrialConfig):
    aggregated_results: list[TrialResult] = []
    stack_id = str(uuid.uuid4())
    for precision in config.precisions:
        results = execute_trial(config, precision, stack_id)
        results = lazy_load_events(config, results)
        aggregated_results.extend(results)

    export_results(aggregated_results, config)
    logging.info(
        "Results are obtainable via stack_id: [%s]",
        stack_id,
    )
