from pathlib import Path
from typing import Sequence
import pyarrow as pa
import pyarrow.parquet as pq

from src.domain.export_service import ExportService


class ParquetExportService(ExportService):
    def __init__(self, location: Path):
        self.location = location

    def export(self, dicts: Sequence[dict], **kwargs):
        if not dicts:
            table = pa.table({})
        else:
            table = pa.Table.from_pylist(dicts)

        pq.write_table(
            table,
            self.location,
            compression="zstd",
            write_statistics=True,
        )
