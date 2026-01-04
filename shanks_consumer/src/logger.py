"""
Logging utility with colored output and function call tracing.
Author: Yadrentsev I. M.
"""

import logging
import sys
from functools import wraps


class ColoredFormatter:
    """Logging formatter that adds colors to log levels for terminal output."""

    COLORS = {
        "DEBUG": "\033[36m",
        "INFO": "\033[32m",
        "WARNING": "\033[33m",
        "ERROR": "\033[31m",
        "CRITICAL": "\033[41m",
        "RESET": "\033[0m",
    }

    def __init__(self, fmt=None, datefmt=None):
        self._formatter = logging.Formatter(fmt, datefmt)

    def format(self, record):
        use_colors = hasattr(sys.stdout, "isatty") and sys.stdout.isatty()

        if use_colors and record.levelname in self.COLORS:
            color = self.COLORS[record.levelname]
            reset = self.COLORS["RESET"]
            record.levelname = f"{color}{record.levelname}{reset}"
            record.msg = f"{color}{record.msg}{reset}"

        return self._formatter.format(record)


def setup_logging(verbose: int, use_colors: bool = True):
    """Sets up logging with specified verbosity and color options."""
    level = (
        logging.DEBUG
        if verbose >= 2
        else logging.INFO if verbose >= 1 else logging.WARNING
    )

    if use_colors is None:
        use_colors = hasattr(sys.stdout, "isatty") and sys.stdout.isatty()

    if use_colors:
        formatter = ColoredFormatter("%(levelname)s: %(message)s")
    else:
        formatter = logging.Formatter("%(levelname)s: %(message)s")

    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(formatter)

    root_logger = logging.getLogger()
    root_logger.setLevel(level)

    for existing_handler in root_logger.handlers[:]:
        root_logger.removeHandler(existing_handler)

    root_logger.addHandler(handler)

    return root_logger


def logged_debug(func):
    """Log function calls and their results at DEBUG level."""
    root_logger = logging.getLogger()

    @wraps(func)
    def wrapper(*args, **kwargs):
        root_logger.debug(
            "Calling %s with args=%s, kwargs=%s",
            func.__name__,
            args,
            kwargs,
        )
        try:
            result = func(*args, **kwargs)

            root_logger.debug(
                "Function %s{func.__name__} returned %s ",
                func.__name__,
                result,
            )
            return result

        except Exception as e:
            root_logger.debug(
                "Function %s raised %s: %s",
                func.__name__,
                type(e).__name__,
                e,
            )
            raise

    return wrapper
