from pathlib import Path
from typing import Sequence

import pandas as pd

from src.domain.export_service import ExportService


class ParquetExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        pd.DataFrame(dicts).to_parquet(
            self.location,
            engine="pyarrow",
            compression="zstd",
            index=False,
            write_statistics=True,
        )
