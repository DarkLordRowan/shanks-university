"""
Configuration models for trial execution.

Author: Yadrentsev I. M.
"""

import json
from pathlib import Path

import dataclasses
from dataclasses import dataclass, field

from src.domain.output_format import OutputFormat
from src.domain.precision import PrecisionType


@dataclass
class NoiseConfig:
    type: str = "Uniform"
    method: str = "jitter"
    param1: float | str = 0.0
    param2: float | str = 0.0
    seed: int = 0

    @classmethod
    def from_dict(cls, data: dict) -> "NoiseConfig":
        return cls(**data)


@dataclass
class FilterConfig:
    type: str
    params: dict = field(default_factory=dict)

    @classmethod
    def from_dict(cls, data: dict) -> "FilterConfig":
        return cls(
            type=data.get("type", "savitzky_golay"),
            params=data.get("params", {})
        )


@dataclass
class TrialConfig:
    """Trial execution configuration model."""

    def __post_init__(self):
        self.series_json = Path(self.series_json)
        self.accel_json = Path(self.accel_json)
        self.noise_json = Path(self.noise_json)
        self.filters_json = Path(self.filters_json)
        self.output_dir = Path(self.output_dir)

        if not self.results_json:
            self.results_json = self.output_dir / "results_json" / f"{self.results_filename}.json"
        if not self.results_csv:
            self.results_csv = self.output_dir / "results_csv" / f"{self.results_filename}.csv"
        if not self.results_parquet:
            self.results_parquet = self.output_dir / "results_parquet" / f"{self.results_filename}.parquet"

        self.results_json = Path(self.results_json)
        self.results_csv = Path(self.results_csv)
        self.results_parquet = Path(self.results_parquet)

        self.precisions = [
            precision if isinstance(precision, PrecisionType) else PrecisionType(precision)
            for precision in self.precisions
        ]

        self.output_formats = [
            OutputFormat(fmt) for fmt in self.output_formats
        ]
        
        # Load noises
        if self.noise_json.exists():
            try:
                data = self.load_json(self.noise_json)
                if "noises" in data:
                    self.noises = data["noises"]
            except Exception:
                pass

        # Load filters
        if self.filters_json.exists():
            try:
                data = self.load_json(self.filters_json)
                if "filters" in data:
                    self.filters = data["filters"]
            except Exception:
                pass

        self.noise_configs = [NoiseConfig.from_dict(n) for n in self.noises]
        self.filter_configs = [FilterConfig.from_dict(f) for f in self.filters]

        self.output_dir.mkdir(parents=True, exist_ok=True)

    verbose: int = 0

    series_json: Path = Path("config/example.json")
    accel_json: Path = Path("config/example.json")
    noise_json: Path = Path("config/example.json")
    filters_json: Path = Path("config/example.json")

    output_dir: Path = Path("output")
    results_filename: str = "results"

    results_json: Path | None = None
    results_csv: Path | None = None
    results_parquet: Path | None = None

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
    
    noises: list[dict] = field(default_factory=list, init=False)
    filters: list[dict] = field(default_factory=list, init=False)

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
        
        # Filter out keys not in dataclass fields to avoid TypeError
        fields = {f.name for f in dataclasses.fields(cls) if f.init}
        filtered_data = {k: v for k, v in data.items() if k in fields}
        
        # If it's a combined file, point parts to itself
        if "series" in data and "series_json" not in data:
            filtered_data["series_json"] = path
        if "methods" in data and "accel_json" not in data:
            filtered_data["accel_json"] = path
        if "noises" in data and "noise_json" not in data:
            filtered_data["noise_json"] = path
        if "filters" in data and "filters_json" not in data:
            filtered_data["filters_json"] = path
            
        return cls(**filtered_data)