import json
from pathlib import Path
from typing import Sequence

from src.domain.export_service import ExportService
from src.domain.trial_result import TrialResult
from src.infra.export.serializer import TrialResultSerializer


class JSONExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, results: Sequence[TrialResult], **kwargs):
        dicts = TrialResultSerializer.to_dict(results)
        with open(self.location, "w", encoding="utf-8") as f:
            json.dump(dicts, f, indent=4, sort_keys=True)
