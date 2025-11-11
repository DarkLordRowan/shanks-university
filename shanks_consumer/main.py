from dotenv import load_dotenv

from src.args import create_parser
from src.commands import handle_run_command, handle_viz_command
from src.config import load_trial_config, load_viz_config
from src.db import setup_mongo_db
from src.logger import setup_logging


def main():
    parser = create_parser()
    args = parser.parse_args()

    load_dotenv()

    match args.command:
        case "run":
            config = load_trial_config(args)
            setup_logging(config.verbose)
            mongo_database = setup_mongo_db(config) if config.with_mongo else None
            handle_run_command(config, mongo_database)
        case "viz":
            config = load_viz_config(args)
            setup_logging(config.verbose)
            mongo_database = setup_mongo_db(config)
            handle_viz_command(config, mongo_database)
        case _:
            parser.print_help()
            exit(1)


if __name__ == "__main__":
    main()
