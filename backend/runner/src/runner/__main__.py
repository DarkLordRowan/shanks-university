"""Run the Shanks experiment runner with ``python -m runner``."""

from shanks.runner.cli import load_config_and_apply_argparse
from shanks.runner.commands.run_cmd import handle_run_command


def main() -> None:
    """Parse command-line arguments and dispatch the requested command."""
    config, args = load_config_and_apply_argparse()

    if args.command == "run":
        handle_run_command(config)
    else:
        raise RuntimeError(f"Unknown command: {args.command}")


if __name__ == "__main__":
    main()
