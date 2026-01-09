"""
CLI parser for trial executor.
Author: Yadrentsev I. M.
"""

import argparse
from pathlib import Path

from src.config.model import TrialConfig

def build_cli_parser() -> argparse.ArgumentParser:
    """Build and return the command-line argument parser."""
    parser = argparse.ArgumentParser(description="Trial executor")

    subparsers = parser.add_subparsers(dest="command", required=True)

    run_parser = subparsers.add_parser("run", help="Run trial executor")

    run_parser.add_argument("--config", type=Path, help="JSON config file")

    return parser

def load_config_and_apply_argparse() -> tuple[TrialConfig, argparse.Namespace]:
    """Load configuration from file and apply command-line argument overrides."""
    parser = build_cli_parser()
    args = parser.parse_args()

    cfg = TrialConfig.load(args.config) if args.config else TrialConfig()
    return cfg, args
