import argparse
import logging
import pathlib
import sys

from dotenv import load_dotenv
from pymongo import MongoClient
from pymongo.database import Database as MongoDatabase
from pymongo.errors import PyMongoError
from src.config import ConfigLoader, TrialConfig
from src.export import ExportTrialResults
from src.loaders import AccelParamLoader, SeriesParamLoader
from src.logger import setup_logging
from src.params import BaseAccelParam, BaseSeriesParam
from src.plot import save_all_plots
from src.trial import ComplexTrial


def load_parameters(config: TrialConfig):
    series_params: list[BaseSeriesParam] = []
    accel_params: list[BaseAccelParam] = []

    if config.series_json.exists():
        logging.info("Loading series from JSON: %s", config.series_json)
        series_params.extend(
            SeriesParamLoader.from_json(config.series_json, config.with_arb)
        )
    else:
        logging.warning("Series JSON file not found: %s", config.series_json)

    if config.series_csv.exists():
        logging.info("Loading series from CSV: %s", config.series_csv)
        series_params.extend(
            SeriesParamLoader.from_csv(config.series_csv, config.with_arb)
        )
    else:
        logging.warning("Series CSV file not found: %s", config.series_csv)

    if config.accel_json.exists():
        logging.info("Loading acceleration methods from: %s", config.accel_json)
        accel_params.extend(
            AccelParamLoader.from_json(config.accel_json, config.with_arb)
        )
    else:
        logging.warning("Acceleration JSON file not found: %s", config.accel_json)

    if not series_params:
        raise ValueError("No series parameters found!")

    logging.info("Loaded %d series parameters", len(series_params))
    logging.info("Loaded %d acceleration parameters", len(accel_params))

    return series_params, accel_params


def execute_trial(config: TrialConfig):
    logging.info("Starting trial execution...")
    logging.info("Arb precision: %s", config.with_arb)
    logging.info("Process count: %d", config.trial_process_count)

    series_params, accel_params = load_parameters(config)

    trial = ComplexTrial(
        series_params,
        accel_params,
        process_count=config.trial_process_count,
        task_timeout=config.trial_task_timeout,
    )
    results = trial.execute()

    return results


def export_results(
    results: list,
    config: TrialConfig,
    mongo_database: MongoDatabase | None = None,
):
    logging.info("Exporting results...")

    results_exporter = ExportTrialResults(results)
    if not config.no_json_export:
        results_exporter.to_json(config.results_json)
        logging.info("Results exported to: %s", config.results_json)
    else:
        logging.info("Skipping export to JSON as requested")

    if not config.no_csv_export:
        results_exporter.to_csv(config.results_csv)
        logging.info("Results exported to: %s", config.results_csv)
    else:
        logging.info("Skipping export to CSV as requested")

    if mongo_database is not None:
        results_exporter.to_mongodb(mongo_database)
        logging.info("Results exported to MongoDB")


def generate_plots(results, config: TrialConfig):
    if config.no_plots:
        logging.info("Skipping plots as requested")
        return

    logging.info("Generating plots...")

    save_all_plots(results, save_dir=config.plots_dir)

    logging.info("Plots saved to: %s", config.plots_dir)


def create_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="pyshanks_consumer CLI")

    config_group = parser.add_argument_group("Configuration")
    config_group.add_argument(
        "--options-json", type=pathlib.Path, help="Load configuration from JSON file"
    )

    input_group = parser.add_argument_group("Input Sources")
    input_group.add_argument(
        "--series-json",
        type=pathlib.Path,
        default=pathlib.Path("data/example.json"),
    )
    input_group.add_argument(
        "--series-csv",
        type=pathlib.Path,
        default=pathlib.Path("data/example_series.csv"),
    )
    input_group.add_argument(
        "--accel-json",
        type=pathlib.Path,
        default=pathlib.Path("data/example.json"),
    )

    output_group = parser.add_argument_group("Output Destinations")
    output_group.add_argument(
        "--output-dir",
        type=pathlib.Path,
        default=pathlib.Path("output"),
    )
    output_group.add_argument(
        "--plots-dir",
        type=pathlib.Path,
        default=pathlib.Path("plots"),
    )
    output_group.add_argument("--results-json", type=pathlib.Path, default=None)
    output_group.add_argument("--results-csv", type=pathlib.Path, default=None)

    execution_group = parser.add_argument_group("Execution Settings")
    execution_group.add_argument(
        "--trial-process-count",
        type=int,
        default=1,
    )

    execution_group.add_argument(
        "--trial-task-timeout",
        type=int,
        default=10,
    )

    feature_group = parser.add_argument_group("Feature Toggles")
    feature_group.add_argument(
        "--no-json-export",
        action="store_true",
    )
    feature_group.add_argument(
        "--no-csv-export",
        action="store_true",
    )
    feature_group.add_argument(
        "--no-events",
        action="store_true",
    )
    feature_group.add_argument(
        "--no-plots",
        action="store_true",
    )
    feature_group.add_argument(
        "--with-arb",
        action="store_true",
    )
    feature_group.add_argument(
        "--with-mongo",
        action="store_true",
    )

    verbosity_group = parser.add_argument_group("Verbosity")
    verbosity_group.add_argument(
        "--verbose",
        "-v",
        action="count",
        default=0,
    )

    return parser


def main():
    parser = create_parser()
    args = parser.parse_args()

    load_dotenv()

    if args.options_json:
        if not args.options_json.exists():
            logging.error("Configuration file not found: %s", args.options_json)
            sys.exit(1)

        config = ConfigLoader.from_json(args.options_json)
        setup_logging(config.verbose)
        logging.info("Loaded configuration from: %s", args.options_json)
    else:
        config = ConfigLoader.from_args(args)
        setup_logging(config.verbose)
        logging.info("Reading CLI arguments")

    logging.info("Configuration Summary:")
    logging.info("  Series JSON: %s", config.series_json)
    logging.info("  Series CSV: %s", config.series_csv)
    logging.info("  Acceleration JSON: %s", config.accel_json)
    logging.info("  Output directory: %s", config.output_dir)
    logging.info("  Plots directory: %s", config.plots_dir)

    mongo_database = None

    if config.with_mongo:
        logging.info("MongoDB export specified, attempting to connect...")
        try:
            mongo_database = MongoClient(
                host=config.mongo_host,
                port=config.mongo_port,
                serverSelectionTimeoutMS=5000,
                connectTimeoutMS=5000,
                username=config.mongo_username,
                password=config.mongo_password,
                authSource=config.mongo_auth_source,
            ).get_database(config.mongo_database)

            mongo_database.client.admin.command("ping")
        except PyMongoError:
            logging.error("Connection to MongoDB failed, skipping this option")
            mongo_database = None
        else:
            logging.info("Connection to MongoDB successful")

    results = execute_trial(config)
    if not config.no_events:
        results = list(map(lambda r: r.load_events(), results))
    export_results(results, config, mongo_database)
    generate_plots(results, config)


if __name__ == "__main__":
    main()
