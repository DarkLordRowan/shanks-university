from dotenv import load_dotenv

from src.args import create_parser
from src.cmd import handle_run_command
from src.config import load_trial_config, load_viz_config

from src.logger import setup_logging


def main():
    parser = create_parser()
    args = parser.parse_args()

    load_dotenv()

    match args.command:
        case "run":
            config = load_trial_config(args)
            setup_logging(config.verbose)
            handle_run_command(config)
        case "viz":
            config = load_viz_config(args)
            setup_logging(config.verbose)
            # handle_viz_command(config)
        case _:
            parser.print_help()
            exit(1)


if __name__ == "__main__":
    main()
