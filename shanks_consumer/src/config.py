import json
import os
import pathlib
from dataclasses import dataclass, field


@dataclass
class TrialConfig:
    series_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))
    series_csv: pathlib.Path = field(default=pathlib.Path("data/example_series.csv"))
    accel_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))

    output_dir: pathlib.Path = field(default=pathlib.Path("output"))
    plots_dir: pathlib.Path = field(default=pathlib.Path("plots"))
    results_json: pathlib.Path | None = None
    results_csv: pathlib.Path | None = None

    trial_process_count: int = 1
    with_arb: bool = False
    no_events: bool = False
    no_plots: bool = False
    no_csv_export: bool = False
    no_json_export: bool = False
    verbose: int = 0

    with_mongo: bool = False

    mongo_host: str = field(
        init=False,
        default_factory=lambda: os.getenv("MONGO_HOST") or "localhost",
    )
    mongo_port: int = field(
        init=False,
        default_factory=lambda: int(os.getenv("MONGO_PORT") or 27017),
    )
    mongo_username: str | None = field(
        init=False, default_factory=lambda: os.getenv("MONGO_USERNAME")
    )
    mongo_password: str | None = field(
        init=False, default_factory=lambda: os.getenv("MONGO_PASSWORD")
    )
    mongo_database: str = field(default="trial_db")

    mongo_auth_source: str = field(default="admin")

    def __post_init__(self):
        if self.results_json is None:
            self.results_json = self.output_dir / "output.json"
        if self.results_csv is None:
            self.results_csv = self.output_dir / "output.csv"

        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.plots_dir.mkdir(parents=True, exist_ok=True)


class ConfigLoader:
    @staticmethod
    def from_args(args) -> TrialConfig:
        return TrialConfig(
            series_json=args.series_json,
            series_csv=args.series_csv,
            accel_json=args.accel_json,
            output_dir=args.output_dir,
            plots_dir=args.plots_dir,
            results_json=args.results_json,
            results_csv=args.results_csv,
            trial_process_count=args.trial_process_count,
            with_arb=args.with_arb,
            no_events=args.no_events,
            no_plots=args.no_plots,
            no_json_export=args.no_json,
            no_csv_export=args.no_csv,
            verbose=args.verbose,
            with_mongo=args.with_mongo,
        )

    @staticmethod
    def from_dict(data: dict) -> TrialConfig:
        return TrialConfig(**data)

    @staticmethod
    def from_json(json_location: pathlib.Path) -> TrialConfig:
        with open(json_location, encoding="utf-8") as f:
            data = json.load(f)

        config_dict = {}
        for key, value in data.items():
            if key.endswith(("_json", "_csv", "_dir")):
                config_dict[key] = pathlib.Path(value)
            else:
                config_dict[key] = value

        return ConfigLoader.from_dict(config_dict)

    @staticmethod
    def default() -> TrialConfig:
        return TrialConfig()
