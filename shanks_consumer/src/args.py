import argparse
import pathlib


def create_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="pyshanks_consumer CLI")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    run_parser = subparsers.add_parser("run", help="Run trials and export results")
    _add_run_arguments(run_parser)

    viz_parser = subparsers.add_parser("viz", help="Visualize existing results")
    _add_viz_arguments(viz_parser)
    return parser


def _add_run_arguments(
    parser: argparse.ArgumentParser,
) -> argparse.ArgumentParser:
    config_group = parser.add_argument_group("Configuration")
    config_group.add_argument(
        "--options-json",
        type=pathlib.Path,
        help="Load configuration from JSON file",
    )
    config_group.add_argument(
        "--precision",
        type=str,
        nargs="+",
        default=None,
        metavar="PRECISION",
        help="Select one or more computation precisions",
    )

    input_group = parser.add_argument_group("Input Sources")
    input_group.add_argument(
        "--series-json",
        type=pathlib.Path,
        default=None,
    )
    input_group.add_argument(
        "--series-csv",
        type=pathlib.Path,
        default=None,
    )
    input_group.add_argument(
        "--accel-json",
        type=pathlib.Path,
        default=None,
    )

    output_group = parser.add_argument_group("Output Destinations")
    output_group.add_argument(
        "--output-dir",
        type=pathlib.Path,
        default=None,
    )
    output_group.add_argument("--results-filename", type=str, default="results", help="Base filename for output files (without extension)")
    output_group.add_argument("--results-json", type=pathlib.Path, default=None)
    output_group.add_argument("--results-csv", type=pathlib.Path, default=None)

    execution_group = parser.add_argument_group("Execution Settings")
    execution_group.add_argument(
        "--trial-process-count",
        type=int,
        default=None,
    )

    execution_group.add_argument(
        "--trial-task-timeout",
        type=int,
        default=None,
    )

    feature_group = parser.add_argument_group("Feature Toggles")
    feature_group.add_argument(
        "--no-events",
        action="store_true",
    )
    feature_group.add_argument(
        "--output-formats",
        nargs="+",
        choices=["json", "csv", "parquet", "mongodb"],
        default=["json", "csv"],
        help="Output formats to generate (default: json csv)",
    )

    verbosity_group = parser.add_argument_group("Verbosity")
    verbosity_group.add_argument(
        "--verbose",
        "-v",
        action="count",
        default=0,
    )

    return parser


def _add_viz_arguments(
    parser: argparse.ArgumentParser,
) -> argparse.ArgumentParser:
    viz_group = parser.add_argument_group("Visualization Options")
    viz_group.add_argument(
        "--stack-id",
        type=str,
        help="Specific stack_id to visualize (otherwise shows all)",
    )
    viz_group.add_argument(
        "--series-name",
        type=str,
        help="Specific series name to draw plots overview",
    )
    viz_group.add_argument(
        "--method-name",
        type=str,
        help="Specific method name to draw plots overview",
    )
    viz_group.add_argument(
        "--with-summary",
        action="store_true",
        help="Shows error summary for given set of results",
    )

    verbosity_group = parser.add_argument_group("Verbosity")
    verbosity_group.add_argument(
        "--verbose",
        "-v",
        action="count",
        default=0,
    )

    return parser
