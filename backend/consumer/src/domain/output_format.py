"""
Output format definitions.
Author: Yadrentsev I. M.
"""

from enum import Enum


class OutputFormat(str, Enum):
    """Enumeration of supported output formats for exporting trial results."""

    JSON = "json"
    CSV = "csv"
