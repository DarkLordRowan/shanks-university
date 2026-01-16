"""
JSON export service implementation.
Author: Yadrentsev I. M.
"""

import json
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService


class JSONExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        """JSON export implementation."""
        output_path = self.location
        
        series = kwargs.get("series")
        if series and len(series) > 0:
            precision = str(series[0].precision)
            stem = output_path.stem
            output_path = output_path.with_name(f"{stem}_{precision}{output_path.suffix}")

        output_path.parent.mkdir(parents=True, exist_ok=True)

        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(dicts, f, indent=4, sort_keys=True)
