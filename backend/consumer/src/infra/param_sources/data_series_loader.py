"""
Data series parameter source implementation.
Author: Yadrentsev I. M.
"""

from typing import Any, Iterable, Mapping

from src.domain.application.param_processing import autowrap
from src.domain.params import SeriesParamJSON
from src.domain.precision import PrecisionType, cast_precision_value
from src.domain.sources import SeriesParamSource
from src.config.model import NoiseConfig


class DataSeriesParamSource(SeriesParamSource):
    def __init__(self, data: Mapping):
        self.data = data

    def load(self, precision: PrecisionType) -> Iterable[SeriesParamJSON]:
        """Loads series parameters from provided data.

        :param precision: The precision type for parameter conversion.
        :type precision: PrecisionType
        :return: An iterable of SeriesParamJSON instances.
        :rtype: Iterable[SeriesParamJSON]
        """
        series_list: list[SeriesParamJSON[Any]] = []

        for series_data in self.data["series"]:
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
