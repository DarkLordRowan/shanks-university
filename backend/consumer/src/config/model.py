"""
Configuration models for trial execution.

Author: Yadrentsev I. M.
"""

import json
from pathlib import Path

from dataclasses import dataclass, field

from src.domain.output_format import OutputFormat
from src.domain.precision import PrecisionType


@dataclass
class TrialConfig:
    """Trial execution configuration model."""

    def __post_init__(self):
        self.series_json = Path(self.series_json)
        self.series_csv = Path(self.series_csv)
        self.accel_json = Path(self.accel_json)
        self.output_dir = Path(self.output_dir)

        self.results_json = (
            Path(self.results_json) if self.results_json else None
        )
        self.results_csv = Path(self.results_csv) if self.results_csv else None

        self.precisions = [
            PrecisionType(precision) for precision in self.precisions
        ]

        self.output_formats = [
            OutputFormat(fmt) for fmt in self.output_formats
        ]

        self.output_dir.mkdir(parents=True, exist_ok=True)

        print(self)

    verbose: int = 0

    series_json: Path = Path("config/example.json")
    series_csv: Path = Path("config/example_series.csv")
    accel_json: Path = Path("config/example.json")

    output_dir: Path = Path("output")
    results_filename: str = "results"

    results_json: Path | None = None
    results_csv: Path | None = None

    trial_process_count: int = 1
    trial_task_timeout: int = 10
    trial_memory_efficient: bool = True

    no_events: bool = False

    precisions: list[PrecisionType] = field(
        default_factory=lambda: [PrecisionType.F64]
    )

    output_formats: list[OutputFormat] = field(
        default_factory=lambda: [OutputFormat.JSON, OutputFormat.CSV]
    )

    @property
    def is_parallel(self) -> int:
        """Is the trial configured to run in parallel?"""
        return self.trial_process_count > 1

    @property
    def precision(self) -> PrecisionType:
        """Get the primary precision for the trial."""
        return self.precisions[0]

    @staticmethod
    def load_json(path: Path) -> dict:
        """Load configuration from a JSON file."""
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)

    @classmethod
    def load(cls, path: Path | None) -> "TrialConfig":
        """Load trial configuration from a file."""
        if path is None:
            return cls()

        if not path.exists():
            raise FileNotFoundError(path)
        elif path.suffix.lower() in {".json"}:
            data = cls.load_json(path)
        else:
            raise RuntimeError(f"Unknown config format: {path.suffix.lower()}")
        return cls(**data)
