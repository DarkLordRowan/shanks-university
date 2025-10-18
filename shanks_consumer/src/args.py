import argparse
import pathlib

from src.run.params import PrecisionType


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


def _add_viz_arguments(
    parser: argparse.ArgumentParser,
) -> argparse.ArgumentParser:
    viz_group = parser.add_argument_group("Visualization Options")
    viz_group.add_argument(
        "--stack-id",
        type=str,
        help="Specific stack_id to visualize (otherwise shows all)",
    )
    # viz_group.add_argument(
    #     "--output-format",
    #     choices=["html", "png", "pdf"],
    #     default="html",
    #     help="Output format for plots",
    # )
    # viz_group.add_argument(
    #     "--interactive",
    #     action="store_true",
    #     help="Launch interactive dashboard",
    # )

    # metric_group = parser.add_argument_group("Metrics")
    # metric_group.add_argument(
    #     "--metric",
    #     choices=["success_rate", "convergence", "errors", "performance"],
    #     default="success_rate",
    #     help="What to visualize",
    # )

    # filter_group = parser.add_argument_group("Filters")
    # filter_group.add_argument(
    #     "--series", type=str, nargs="+", help="Filter by series names"
    # )
    # filter_group.add_argument(
    #     "--methods", type=str, nargs="+", help="Filter by method names"
    # )
    verbosity_group = parser.add_argument_group("Verbosity")
    verbosity_group.add_argument(
        "--verbose",
        "-v",
        action="count",
        default=0,
    )

    return parser
