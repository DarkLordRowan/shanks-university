from src.infra.param_sources.csv_series_loader import CSVSeriesParamSource
from src.infra.param_sources.json_accel_loader import JSONAccelParamSource
from src.infra.param_sources.json_series_loader import JSONSeriesParamSource

PARAM_SERIES_SOURCE_BUILDERS = [
    lambda cfg: (
        JSONSeriesParamSource(cfg.series_json)
        if cfg.series_json and cfg.series_json.exists()
        else None
    ),
    lambda cfg: (
        CSVSeriesParamSource(cfg.series_csv)
        if cfg.series_csv and cfg.series_csv.exists()
        else None
    ),
]

PARAM_ACCELS_SOURCE_BUILDERS = [
    lambda cfg: (
        JSONAccelParamSource(cfg.accel_json)
        if cfg.accel_json and cfg.accel_json.exists()
        else None
    ),
]
