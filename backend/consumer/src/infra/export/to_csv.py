"""
JSON export service implementation.
Author: Yadrentsev I. M.
"""

import csv
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService


class CSVExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        """CSV export implementation."""
        if not dicts:
            return

        output_path = self.location
        series = kwargs.get("series")
        if series and len(series) > 0:
            precision = str(series[0].precision)
            stem = output_path.stem
            output_path = output_path.with_name(f"{stem}_{precision}{output_path.suffix}")

        output_path.parent.mkdir(parents=True, exist_ok=True)

        with open(output_path, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=dicts[0].keys())
            writer.writeheader()
            writer.writerows(dicts)
