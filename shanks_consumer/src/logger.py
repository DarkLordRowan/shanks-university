import logging
import sys
from functools import wraps


class ColoredFormatter(logging.Formatter):
    COLORS = {
        "DEBUG": "\033[36m",  # Cyan
        "INFO": "\033[32m",  # Green
        "WARNING": "\033[33m",  # Yellow
        "ERROR": "\033[31m",  # Red
        "CRITICAL": "\033[41m",  # Red background
        "RESET": "\033[0m",  # Reset
    }

    def format(self, record):
        use_colors = hasattr(sys.stdout, "isatty") and sys.stdout.isatty()

        if use_colors and record.levelname in self.COLORS:
            color = self.COLORS[record.levelname]
            reset = self.COLORS["RESET"]
            record.levelname = f"{color}{record.levelname}{reset}"
            record.msg = f"{color}{record.msg}{reset}"

        return super().format(record)


def setup_logging(verbose: int, use_colors: bool = True):
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


def logged_debug(logger_name=None):

    def decorator(func):
        logger = logging.getLogger(logger_name or func.__module__)

        @wraps(func)
        def wrapper(*args, **kwargs):
            logger.debug(
                "Calling %s with args=%s, kwargs=%s",
                func.__name__,
                args,
                kwargs,
            )
            try:
                result = func(*args, **kwargs)

                logger.debug(
                    "Function %s{func.__name__} returned %s ",
                    func.__name__,
                    result,
                )
                return result

            except Exception as e:
                logger.debug(
                    "Function %s raised %s: %s",
                    func.__name__,
                    type(e).__name__,
                    e,
                )
                raise

        return wrapper

    return decorator
