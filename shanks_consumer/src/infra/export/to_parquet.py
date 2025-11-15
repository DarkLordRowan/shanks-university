from pathlib import Path
from typing import Sequence

import pandas as pd

from src.domain.export_service import ExportService
from src.domain.trial_result import TrialResult
from src.infra.export.serializer import TrialResultSerializer


class ParquetExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, results: Sequence[TrialResult], **kwargs):
        df = pd.DataFrame(TrialResultSerializer.to_dict(results))
        df.to_parquet(
            self.location,
            engine="pyarrow",
            compression="zstd",
            index=False,
            write_statistics=True,
        )
