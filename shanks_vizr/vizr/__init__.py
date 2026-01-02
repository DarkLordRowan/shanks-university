"""
Author: Sobolev Y. A.
Description: Initialization module for the vizr package, exposing key components.
"""
# Expose the main components to the package level so they can be imported directly
# like `from vizr import DataLoader` instead of `from vizr.data_loader import DataLoader`.
from .data_loader import DataLoader, SeriesRecord, AccelRecord
from .symlog import Scientific
from .main import main