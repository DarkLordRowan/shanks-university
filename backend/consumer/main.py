from src.cli import load_config_and_apply_argparse
from src.commands.run_cmd import handle_run_command


def main():
    config, args = load_config_and_apply_argparse()

    if args.command == "run":
        handle_run_command(config)
    else:
        raise RuntimeError(f"Unknown command: {args.command}")


if __name__ == "__main__":
    main()
    print("OK")
