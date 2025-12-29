import json
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService


class JSONExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        with open(self.location, "a", encoding="utf-8") as f:
            json.dump(dicts, f, indent=4, sort_keys=True)
