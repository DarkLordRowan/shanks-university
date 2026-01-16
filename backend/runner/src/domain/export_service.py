"""
Export service protocol definition.
Author: Yadrentsev I. M.
"""

from typing import Any, Protocol, Sequence


class ExportService(Protocol):
    """Export service protocol.

    Designs a protocol for exporting a sequence of dictionaries to a specific format.
    """

    def export(self, dicts: Sequence[dict], **kwargs: Any) -> None: ...
