import json
import os
import pathlib
from abc import abstractmethod
from collections.abc import Sequence
from dataclasses import dataclass, field, replace

from src.run.params import PrecisionType

PrecisionValue = PrecisionType | str
PrecisionCollection = Sequence[PrecisionValue] | PrecisionValue | None


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

    precisions: PrecisionCollection = field(
        default_factory=lambda: (PrecisionType.F64,)
    )
    no_events: bool = False
    no_csv_export: bool = False
    no_json_export: bool = False

    with_mongo: bool = False

    def __post_init__(self):
        self.precisions = self._normalize_precisions(self.precisions)
        if self.results_json is None:
            self.results_json = self.output_dir / "output.json"
        if self.results_csv is None:
            self.results_csv = self.output_dir / "output.csv"

        self.output_dir.mkdir(parents=True, exist_ok=True)

    @property
    def precision(self) -> PrecisionType:
        return self.precisions[0]

    @staticmethod
    def _coerce_precision(value: PrecisionValue) -> PrecisionType:
        if isinstance(value, PrecisionType):
            return value
        if isinstance(value, str):
            normalized = value.upper()
            try:
                return PrecisionType[normalized]
            except KeyError:
                for precision in PrecisionType:
                    if (
                        value == precision.value
                        or value.lower() == precision.value.lower()
                    ):
                        return precision
        raise ValueError(f"Unsupported precision: {value}")

    @classmethod
    def _normalize_precisions(
        cls, raw_precisions: PrecisionCollection
    ) -> tuple[PrecisionType, ...]:
        if raw_precisions is None:
            candidates: Sequence[PrecisionValue] = (PrecisionType.F64,)
        elif isinstance(raw_precisions, (PrecisionType, str)):
            candidates = (raw_precisions,)
        else:
            candidates = raw_precisions

        normalized: list[PrecisionType] = []
        for candidate in candidates:
            precision = cls._coerce_precision(candidate)
            if precision not in normalized:
                normalized.append(precision)

        if not normalized:
            raise ValueError("At least one precision must be provided")

        return tuple(normalized)


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
        overrides = TrialConfigLoader.overrides_from_args(args)
        base_config = TrialConfigLoader.default()
        if overrides:
            return replace(base_config, **overrides)
        return base_config

    @staticmethod
    def from_dict(data: dict) -> TrialConfig:
        config_data = dict(data)
        has_precision = "precision" in config_data
        has_precisions = "precisions" in config_data
        if has_precision and has_precisions:
            raise ValueError(
                "Configuration cannot define both 'precision' and 'precisions'"
            )
        if has_precision:
            config_data["precisions"] = config_data.pop("precision")
        return TrialConfig(**config_data)

    @staticmethod
    def default() -> TrialConfig:
        return TrialConfig()

    @staticmethod
    def overrides_from_args(args) -> dict[str, object]:
        overrides: dict[str, object] = {}

        if getattr(args, "series_json", None) is not None:
            overrides["series_json"] = args.series_json
        if getattr(args, "series_csv", None) is not None:
            overrides["series_csv"] = args.series_csv
        if getattr(args, "accel_json", None) is not None:
            overrides["accel_json"] = args.accel_json
        if getattr(args, "output_dir", None) is not None:
            overrides["output_dir"] = args.output_dir
        if getattr(args, "results_json", None) is not None:
            overrides["results_json"] = args.results_json
        if getattr(args, "results_csv", None) is not None:
            overrides["results_csv"] = args.results_csv

        if getattr(args, "trial_process_count", None) is not None:
            overrides["trial_process_count"] = args.trial_process_count
        if getattr(args, "trial_task_timeout", None) is not None:
            overrides["trial_task_timeout"] = args.trial_task_timeout

        if getattr(args, "precision", None) is not None:
            overrides["precisions"] = args.precision

        if getattr(args, "no_events", False):
            overrides["no_events"] = True
        if getattr(args, "no_json_export", False):
            overrides["no_json_export"] = True
        if getattr(args, "no_csv_export", False):
            overrides["no_csv_export"] = True

        if getattr(args, "with_mongo", False):
            overrides["with_mongo"] = True

        if getattr(args, "verbose", 0) > 0:
            overrides["verbose"] = args.verbose

        return overrides


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
    if hasattr(args, "options_json") and args.options_json:
        config = TrialConfigLoader.from_json(args.options_json)
    else:
        config = TrialConfigLoader.default()

    overrides = TrialConfigLoader.overrides_from_args(args)
    if overrides:
        config = replace(config, **overrides)

    return config


def load_viz_config(args) -> VizConfig:
    if hasattr(args, "options_json") and args.options_json:
        config = VizConfigLoader.from_json(args.options_json)
        return config

    config = VizConfigLoader.from_args(args)

    return config
