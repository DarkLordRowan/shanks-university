import logging
import sys


def setup_logging(verbose: int):
    level = (
        logging.DEBUG
        if verbose >= 2
        else logging.INFO if verbose >= 1 else logging.WARNING
    )
    logging.basicConfig(
        level=level,
        format="%(levelname)s: %(message)s",
        handlers=[logging.StreamHandler(sys.stdout)],
    )
