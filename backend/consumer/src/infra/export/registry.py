"""
Export service registry and builders.
Author: Yadrentsev I. M.
"""

from src.config.model import TrialConfig
from src.domain.output_format import OutputFormat
from src.infra.export.to_csv import CSVExportService
from src.infra.export.to_json import JSONExportService
from src.infra.export.to_parquet import ParquetExportService

def build_json(cfg: TrialConfig) -> JSONExportService | None:
    """Builds a JSON export service if configured."""
    if not cfg.results_json:
        return None
    return JSONExportService(location=cfg.results_json)


def build_csv(cfg: TrialConfig) -> CSVExportService | None:
    """Builds a CSV export service if configured."""
    if not cfg.results_csv:
        return None
    return CSVExportService(location=cfg.results_csv)


def build_parquet(cfg: TrialConfig) -> ParquetExportService | None:
    """Builds a Parquet export service if configured."""
    if not cfg.results_parquet:
        return None
    return ParquetExportService(location=cfg.results_parquet)


EXPORT_BUILDERS = {
    OutputFormat.JSON: build_json,
    OutputFormat.CSV: build_csv,
    OutputFormat.PARQUET: build_parquet,
}
