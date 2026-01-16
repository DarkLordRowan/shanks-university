"""
Data series parameter source implementation.
Author: Yadrentsev I. M.
"""

from pathlib import Path
from typing import Any, Iterable, Mapping

from src.domain.application.param_processing import autowrap
from src.domain.params import BaseSeriesParam, SeriesParamJSON
from src.domain.precision import PrecisionType, cast_precision_value
from src.domain.sources import SeriesParamSource
from src.config.model import NoiseConfig
from src.infra.param_sources.csv_series_loader import CSVSeriesParamSource


class DataSeriesParamSource(SeriesParamSource):
    def __init__(self, data: Mapping, context_path: Path | None = None):
        self.data = data
        self.context_path = context_path

    def load(self, precision: PrecisionType) -> Iterable[BaseSeriesParam]:
        """Loads series parameters from provided data.

        :param precision: The precision type for parameter conversion.
        :type precision: PrecisionType
        :return: An iterable of BaseSeriesParam instances.
        :rtype: Iterable[BaseSeriesParam]
        """
        series_list: list[BaseSeriesParam] = []

        for series_data in self.data["series"]:
            if isinstance(series_data, str):
                csv_path = Path(series_data)
                if not csv_path.is_absolute() and self.context_path:
                    csv_path = self.context_path / csv_path
                
                if csv_path.exists():
                    loader = CSVSeriesParamSource(csv_path)
                    series_list.extend(loader.load(precision))
                continue

            args = series_data.get("args", {})
            processed = self._process_args(args, precision)
            
            series_list.append(
                SeriesParamJSON(
                    precision=precision,
                    name=series_data["name"],
                    args=processed,
                )
            )

        return series_list

    def load_noises(self) -> list[NoiseConfig]:
        """Loads noise configurations from provided data.

        :return: A list of NoiseConfig instances.
        :rtype: list[NoiseConfig]
        """
        return [
            NoiseConfig.from_dict(n) for n in self.data.get("noises", [])
        ]

    def _process_args(self, args: Any, precision: PrecisionType):
        """
        Processes argument values based on their keys and the specified precision.

        THe following keys are specifically handled:
        - "vecSize", "addKParameter", "m", "b": values are converted to integers.
        - Other keys: values are converted to the appropriate real subtype based on the given precision.

        :param args: The raw argument values.
        :type args: Any
        :param precision: The precision type for value conversion.
        :type precision: PrecisionType
        """
        if not args:
            return {}

        if not isinstance(args, dict):
            args = {"x": autowrap(args)}

        processed = {}
        for key, value in args.items():
            values = autowrap(value)
            if key in {"vecSize", "addKParameter", "m", "b"}:
                processed[key] = [int(v) for v in values]
            else:
                processed[key] = [cast_precision_value(precision, v) for v in values]
        return processed
