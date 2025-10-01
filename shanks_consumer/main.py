import argparse
import pathlib
import sys
import logging

from src.loaders import (
    AccelParamLoader,
    SeriesParamLoader
)
from src.trial import ComplexTrial
from src.export import ExportTrialResults, ExportTrialEvents
from src.events import TrialEventScanner
from src.plot import save_all_plots
from src.logger import setup_logging
from src.config import TrialConfig, ConfigLoader


def load_parameters(config: TrialConfig):
    series_params = []
    accel_params = []

    if config.series_json.exists():
        logging.info("Loading series from JSON: %s", config.series_json)
        series_params.extend(SeriesParamLoader.from_json(config.series_json, config.with_arb))
    else:
        logging.warning("Series JSON file not found: %s", config.series_json)

    if config.series_csv.exists():
        logging.info("Loading series from CSV: %s", config.series_csv)
        series_params.extend(SeriesParamLoader.from_csv(config.series_csv, config.with_arb))
    else:
        logging.warning("Series CSV file not found: %s", config.series_csv)

    if config.accel_json.exists():
        logging.info(
            "Loading acceleration methods from: %s", config.accel_json
        )
        accel_params.extend(AccelParamLoader.from_json(config.accel_json, config.with_arb))
    else:
        logging.warning(
            "Acceleration JSON file not found: %s", config.accel_json
        )

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

    trial = ComplexTrial(series_params, accel_params, process_count=config.trial_process_count)
    results = trial.execute()

    return results


def export_results(results, config: TrialConfig):
    logging.info("Exporting results...")

    results_exporter = ExportTrialResults(results)
    results_exporter.to_json(config.results_json)
    results_exporter.to_csv(config.results_csv)

    logging.info(
        "Results exported to: %s, %s", config.results_json, config.results_csv
    )


def generate_plots(results, config: TrialConfig):
    if config.no_plots:
        logging.info("Skipping plots as requested")
        return

    logging.info("Generating plots...")

    save_all_plots(results, save_dir=config.plots_dir)

    logging.info("Plots saved to: %s", config.plots_dir)


def scan_events(results, config: TrialConfig):
    if config.no_events:
        logging.info("Skipping event scanning as requested")
        return

    logging.info("Scanning for events...")

    scanner = TrialEventScanner(results)
    events = scanner.execute()

    events_exporter = ExportTrialEvents(events)
    events_exporter.to_json(config.events_json)
    events_exporter.to_csv(config.events_csv)

    logging.info(
        "Events exported to: %s, %s", config.events_json, config.events_csv
    )

def create_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="pyshanks_consumer CLI")
    
    config_group = parser.add_argument_group("Configuration")
    config_group.add_argument(
        "--options-json",
        type=pathlib.Path,
        help="Load configuration from JSON file"
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
    output_group.add_argument("--events-json", type=pathlib.Path, default=None)
    output_group.add_argument("--events-csv", type=pathlib.Path, default=None)
    
    execution_group = parser.add_argument_group("Execution Settings")
    execution_group.add_argument(
        "--trial-process-count", 
        type=int, 
        default=1,
    )
    
    feature_group = parser.add_argument_group("Feature Toggles")
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
    
    verbosity_group = parser.add_argument_group("Verbosity")
    verbosity_group.add_argument(
        "--verbose", "-v", 
        action="count", 
        default=0,
    )
    
    return parser


def main():
    parser = create_parser()
    args = parser.parse_args()

    if args.options_json:
        if not args.options_json.exists():
            logging.error(
                "Configuration file not found: %s", args.options_json
            )
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

    results = execute_trial(config)
    export_results(results, config)
    generate_plots(results, config)
    scan_events(results, config)


if __name__ == "__main__":
    main()
