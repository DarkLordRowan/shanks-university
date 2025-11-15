from src.db import setup_mongo_db
from src.domain.output_format import OutputFormat
from src.infra.export.to_csv import CSVExportService
from src.infra.export.to_json import JSONExportService
from src.infra.export.to_mongo import MongoExportService
from src.infra.export.to_parquet import ParquetExportService


def build_json(cfg) -> JSONExportService | None:
    if not cfg.results_json:
        return None
    return JSONExportService(location=cfg.results_json)


def build_csv(cfg) -> CSVExportService | None:
    if not cfg.results_csv:
        return None
    return CSVExportService(location=cfg.results_csv)


def build_parquet(cfg) -> ParquetExportService | None:
    if not cfg.results_parquet:
        return None
    return ParquetExportService(location=cfg.results_parquet)


def build_mongodb(cfg) -> MongoExportService | None:
    mongo_db = setup_mongo_db(cfg)
    if mongo_db is None:
        return None
    return MongoExportService(
        mongo_database=mongo_db,
        collection_name=cfg.mongo_collection,
    )


EXPORT_BUILDERS = {
    OutputFormat.JSON: build_json,
    OutputFormat.CSV: build_csv,
    OutputFormat.PARQUET: build_parquet,
    OutputFormat.MONGODB: build_mongodb,
}
