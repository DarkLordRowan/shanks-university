"""
Data serializer protocol definition.
Author: Yadrentsev I. M.
"""

from typing import Protocol, Sequence

from src.domain.trial_result import TrialResult


class DataSerializer(Protocol):
    """Data serializer protocol.

    Designs a protocol for serializing trial results into a sequence of dictionaries.
    """

    def to_dict(self, results: Sequence[TrialResult]) -> Sequence[dict]: ...
