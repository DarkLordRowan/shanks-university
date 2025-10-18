import json
import os
import pathlib
from abc import abstractmethod
from dataclasses import dataclass, field


@dataclass
class BaseConfig:
    verbose: int = 0


@dataclass
class MongoConfig:
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
    mongo_collection: str = field(default="trial_results")
    mongo_auth_source: str = field(default="admin")


@dataclass
class TrialConfig(BaseConfig, MongoConfig):
    series_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))
    series_csv: pathlib.Path = field(default=pathlib.Path("data/example_series.csv"))
    accel_json: pathlib.Path = field(default=pathlib.Path("data/example.json"))

    output_dir: pathlib.Path = field(default=pathlib.Path("output"))
    results_json: pathlib.Path | None = None
    results_csv: pathlib.Path | None = None

    trial_process_count: int = 1
    trial_task_timeout: int = 10

    with_arb: bool = False
    no_events: bool = False
    no_csv_export: bool = False
    no_json_export: bool = False

    with_mongo: bool = False

    def __post_init__(self):
        if self.results_json is None:
            self.results_json = self.output_dir / "output.json"
        if self.results_csv is None:
            self.results_csv = self.output_dir / "output.csv"

        self.output_dir.mkdir(parents=True, exist_ok=True)


@dataclass
class VizConfig(BaseConfig, MongoConfig):
    stack_id: str | None = None


class BaseConfigLoader[T]:
    @staticmethod
    @abstractmethod
    def from_args(args) -> T:
        pass

    @staticmethod
    @abstractmethod
    def from_dict(data: dict) -> T:  # type: ignore
        pass

    @staticmethod
    def _process_path_fields(data: dict) -> dict:
        config_dict = {}
        for key, value in data.items():
            if key.endswith(("_json", "_csv", "_dir")):
                config_dict[key] = pathlib.Path(value)
            else:
                config_dict[key] = value
        return config_dict

    @classmethod
    def from_json(cls, json_location: pathlib.Path) -> T:  # type: ignore
        with open(json_location, encoding="utf-8") as f:
            data = json.load(f)

        config_dict = cls._process_path_fields(data)
        return cls.from_dict(config_dict)

    @staticmethod
    @abstractmethod
    def default() -> T:  # type: ignore
        pass


class TrialConfigLoader(BaseConfigLoader[TrialConfig]):
    @staticmethod
    def from_args(args) -> TrialConfig:
        return TrialConfig(
            series_json=args.series_json,
            series_csv=args.series_csv,
            accel_json=args.accel_json,
            output_dir=args.output_dir,
            results_json=args.results_json,
            results_csv=args.results_csv,
            trial_process_count=args.trial_process_count,
            trial_task_timeout=args.trial_task_timeout,
            with_arb=args.with_arb,
            no_events=args.no_events,
            no_json_export=args.no_json_export,
            no_csv_export=args.no_csv_export,
            verbose=args.verbose,
            with_mongo=args.with_mongo,
        )

    @staticmethod
    def from_dict(data: dict) -> TrialConfig:
        return TrialConfig(**data)

    @staticmethod
    def default() -> TrialConfig:
        return TrialConfig()


class VizConfigLoader(BaseConfigLoader[VizConfig]):
    @staticmethod
    def from_args(args) -> VizConfig:
        return VizConfig(
            stack_id=args.stack_id,
        )

    @staticmethod
    def from_dict(data: dict) -> VizConfig:
        return VizConfig(**data)

    @staticmethod
    def default() -> VizConfig:
        return VizConfig()


def load_trial_config(args) -> TrialConfig:
    if hasattr(args, "options.json") and args.options_json:
        config = TrialConfigLoader.from_json(args.options_json)
        return config

    config = TrialConfigLoader.from_args(args)

    return config


def load_viz_config(args) -> VizConfig:
    if hasattr(args, "options.json") and args.options_json:
        config = VizConfigLoader.from_json(args.options_json)
        return config

    config = VizConfigLoader.from_args(args)

    return config
