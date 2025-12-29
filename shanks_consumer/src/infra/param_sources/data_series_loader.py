from typing import Any, Iterable, Mapping

from src.domain.application.param_processing import autowrap
from src.domain.params import SeriesParamJSON
from src.domain.precision import PrecisionType, cast_precision_value
from src.domain.sources import SeriesParamSource


class DataSeriesParamSource(SeriesParamSource):
    def __init__(self, data: Mapping):
        self.data = data

    def load(self, precision: PrecisionType) -> Iterable[SeriesParamJSON]:
        series_list: list[SeriesParamJSON[Any]] = []

        for series_data in self.data["series"]:
            args = series_data.get("args", {})
            processed = self._process_args(args, precision)

            series_list.append(
                SeriesParamJSON(
                    precision=precision,
                    name=series_data["name"] + precision.value,
                    args=processed,
                )
            )

        return series_list

    def _process_args(self, args: Any, precision: PrecisionType):
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
