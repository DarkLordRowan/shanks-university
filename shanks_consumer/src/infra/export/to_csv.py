import csv
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService


class CSVExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        if not dicts:
            return

        with open(self.location, "a", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=dicts[0].keys())
            writer.writeheader()
            writer.writerows(dicts)
