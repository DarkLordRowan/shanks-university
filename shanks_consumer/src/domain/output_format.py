from enum import Enum


class OutputFormat(str, Enum):
    JSON = "json"
    CSV = "csv"
    PARQUET = "parquet"
    PARQUET_SPLIT = "parquet_split"
    MONGODB = "mongodb"
