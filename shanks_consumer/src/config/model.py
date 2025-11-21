import json
import os
from pathlib import Path

import yaml
from pydantic import BaseModel, Field

from src.domain.application.result_view import ResultViewKind
from src.domain.output_format import OutputFormat
from src.domain.precision import PrecisionType


class MongoConfig(BaseModel):
    host: str = Field(default_factory=lambda: os.getenv("MONGO_HOST", "localhost"))
    port: int = Field(default_factory=lambda: int(os.getenv("MONGO_PORT", "27017")))
    username: str | None = Field(default_factory=lambda: os.getenv("MONGO_USERNAME"))
    password: str | None = Field(default_factory=lambda: os.getenv("MONGO_PASSWORD"))
    database: str = "trial_db"
    collection: str = "trial_results"
    auth_source: str = "admin"


class OutputConfig(BaseModel):
    parquet_collection: str = "trial_results"


class TrialConfig(BaseModel):
    verbose: int = 0

    series_json: Path = Path("config/example.json")
    series_csv: Path = Path("config/example_series.csv")
    accel_json: Path = Path("config/example.json")

    output_dir: Path = Path("output")
    results_filename: str = "results"
    results_json: Path | None = None
    results_csv: Path | None = None
    results_parquet: Path | None = None

    trial_process_count: int = 1
    trial_task_timeout: int = 10
    trial_result_view: ResultViewKind = ResultViewKind.FULL
    trial_memory_efficient: bool = True

    no_events: bool = False

    precisions: list[PrecisionType] = [PrecisionType.F64]

    output_formats: list[OutputFormat] = [
        OutputFormat.JSON,
        OutputFormat.CSV,
    ]

    mongo: MongoConfig = MongoConfig()
    output: OutputConfig = OutputConfig()

    @property
    def is_parallel(self) -> int:
        return self.trial_process_count > 1

    @property
    def precision(self) -> PrecisionType:
        return self.precisions[0]

    @staticmethod
    def load_json(path: Path) -> dict:
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)

    @staticmethod
    def load_yaml(path: Path) -> dict:
        with open(path, "r", encoding="utf-8") as f:
            return yaml.safe_load(f)

    @classmethod
    def load(cls, path: Path | None) -> "TrialConfig":
        if path is None:
            return cls()

        if not path.exists():
            raise FileNotFoundError(path)
        if path.suffix.lower() in {".yml", ".yaml"}:
            data = cls.load_yaml(path)
        elif path.suffix.lower() in {".json"}:
            data = cls.load_json(path)
        else:
            raise RuntimeError(f"Unknown config format: {path.suffix.lower()}")
        return cls(**data)
