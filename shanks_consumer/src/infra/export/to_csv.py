import csv
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService
from src.domain.trial_result import TrialResult
from src.infra.export.serializer import TrialResultSerializer


class CSVExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, results: Sequence[TrialResult], **kwargs):
        dicts = TrialResultSerializer.to_dict(results)
        if not dicts:
            return

        with open(self.location, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=dicts[0].keys())
            writer.writeheader()
            writer.writerows(dicts)
