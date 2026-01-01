from dotenv import load_dotenv

from src.cli import load_config_and_apply_argparse
from src.cmd.run_cmd import handle_run_command

import pyshanks as ps


#def main():
#    load_dotenv()
#
#    config, args = load_config_and_apply_argparse()
#
#    if args.command == "run":
#        handle_run_command(config)
#    else:
#        raise RuntimeError(f"Unknown command: {args.command}")


if __name__ == "__main__":
    #main()
    data = ps.ExpSeriesCArb().generateSeries(ps.CArb(ps.Arb("1.01", 300), ps.Arb("2.05", 200)), 10000)
    print(data.Sn)