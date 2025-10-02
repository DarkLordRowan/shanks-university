import json
import pathlib
from dataclasses import dataclass, field
from typing import Optional


@dataclass
class TrialConfig:
    series_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))
    series_csv: pathlib.Path = field(default=pathlib.Path("data/example_series.csv"))
    accel_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))

    output_dir: pathlib.Path = field(default=pathlib.Path("output"))
    plots_dir: pathlib.Path = field(default=pathlib.Path("plots"))
    results_json: Optional[pathlib.Path] = None
    results_csv: Optional[pathlib.Path] = None
    events_json: Optional[pathlib.Path] = None
    events_csv: Optional[pathlib.Path] = None

    trial_process_count: int = 1
    with_arb: bool = False
    no_events: bool = False
    no_plots: bool = False
    verbose: int = 0

    def __post_init__(self):
        if self.results_json is None:
            self.results_json = self.output_dir / "output.json"
        if self.results_csv is None:
            self.results_csv = self.output_dir / "output.csv"
        if self.events_json is None:
            self.events_json = self.output_dir / "events.json"
        if self.events_csv is None:
            self.events_csv = self.output_dir / "events.csv"

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
            events_json=args.events_json,
            events_csv=args.events_csv,
            trial_process_count=args.trial_process_count,
            with_arb=args.with_arb,
            no_events=args.no_events,
            no_plots=args.no_plots,
            verbose=args.verbose,
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
