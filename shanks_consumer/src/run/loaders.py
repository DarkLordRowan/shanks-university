import csv
import json
import pathlib
from typing import Any, Iterable, Mapping

import pyshanks as ps
from src.run.params import AccelParamJSON, SeriesParamCSV, SeriesParamJSON, PrecisionConfig
from src.run.precision import (
    PrecisionType,
    is_arb_precision,
)


def decide_encoder(precision: PrecisionConfig):
    if is_arb_precision(precision):
        arb_precision = precision.precision or 50
        
        class _Decoder(json.JSONDecoder):
            def __init__(self, *args, **kwargs):
                super().__init__(
                    parse_float=lambda x: ps.Arb(float(x), arb_precision), 
                    parse_int=lambda x: ps.Arb(float(x), arb_precision), 
                    *args, **kwargs
                )
 
        return _Decoder
 
    return None


def autowrap(x: Any) -> Iterable[Any]:
    if x is None:
        return []

    if isinstance(x, dict) and all(k in x for k in ("start", "stop", "step")):
        start = _ensure_number(x["start"])
        stop = _ensure_number(x["stop"])
        step = _ensure_number(x["step"])
        return _generate_range_values(start, stop, step)

    if isinstance(x, (str, bytes, bytearray)):
        return [x]

    if isinstance(x, Iterable) and not isinstance(x, (dict, str, bytes, bytearray)):
        return [_ensure_number(value) for value in x]

    return [_ensure_number(x)]


def _ensure_number(value: Any) -> Any:
    if isinstance(value, bool):
        return value
    if isinstance(value, (int, float)):
        return value
    if isinstance(value, str):
        try:
            if any(sep in value for sep in (".", "e", "E")):
                return float(value)
            return int(value)
        except ValueError:
            return value
    try:
        return float(value)
    except (TypeError, ValueError):
        return value


def _generate_range_values(start: float, stop: float, step: float) -> list[float]:
    if step == 0:
        raise ValueError("Range specification has zero step")

    steps = int((stop - start) / step)
    return [start + i * step for i in range(steps)]


class SeriesParamLoader:
    @staticmethod
    def from_data(
        data: Mapping[str, Any],
        precision: PrecisionConfig = PrecisionConfig(PrecisionType.F64),
    ) -> list[SeriesParamJSON]:
        series_list: list[SeriesParamJSON[Any]] = []
        for series_data in data["series"]:
            args = series_data.get("args", {})
            processed_args = SeriesParamLoader._process_series_args(args, precision)
            series_list.append(
                SeriesParamJSON(
                    name=series_data.get("name"),
                    args=processed_args,
                    precision=precision,
                )
            )
        return series_list

    @staticmethod
    def _process_series_args(
        args: Any, precision: PrecisionConfig
    ) -> dict[str, Iterable[Any]]:
        if not args:
            return {}

        if not isinstance(args, dict):
            args = {"x": autowrap(args)}

        processed: dict[str, Iterable[Any]] = {}
        for key, value in args.items():
            processed[str(key)] = list(autowrap(value))
        return processed

    @staticmethod
    def from_json(
        json_location: pathlib.Path,
        precision: PrecisionConfig = PrecisionConfig(PrecisionType.F64),
    ) -> list[SeriesParamJSON]:
        with open(json_location, encoding="utf-8") as f:
            data = json.load(f, cls=decide_encoder(precision))
        return SeriesParamLoader.from_data(data, precision)

    @staticmethod
    def from_csv(
        csv_location: pathlib.Path,
        precision: PrecisionConfig = PrecisionConfig(PrecisionType.F64),
    ) -> list[SeriesParamCSV]:
        results: list[SeriesParamCSV] = []
        with open(csv_location, encoding="utf-8") as f:
            reader = csv.reader(f)
            for i, row in enumerate(reader, 1):
                results.append(
                    SeriesParamCSV(
                        location=pathlib.Path(csv_location),
                        row=i,
                        precision=precision,
                        raw_values=tuple(row),
                    )
                )
        return results


class AccelParamLoader:
    @staticmethod
    def from_json(
        json_location: pathlib.Path,
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[AccelParamJSON]:
        with open(json_location, encoding="utf-8") as f:
            data = json.load(f, cls=decide_encoder(precision))
        return AccelParamLoader.from_data(data, precision)

    @staticmethod
    def from_data(
        data: Mapping[str, Any],
        precision: PrecisionType = PrecisionType.F64,
    ) -> list[AccelParamJSON]:
        methods_list: list[AccelParamJSON[Any]] = []

        for method_data in data["methods"]:
            n_value = [
                int(x) if not isinstance(x, int) else x
                for x in autowrap(method_data["n"])
            ]
            m_value = [
                int(x) if not isinstance(x, int) else x
                for x in autowrap(method_data["m"])
            ]

            args = {}
            for key, value in method_data.get("args", {}).items():
                args[str(key)] = list(autowrap(value))

            methods_list.append(
                AccelParamJSON(
                    name=method_data["name"],
                    n=n_value,
                    m=m_value,
                    init_args=args,
                    precision=precision,
                )
            )

        return methods_list


get_series_params_from_json = SeriesParamLoader.from_json
get_accel_params_from_json = AccelParamLoader.from_json
get_series_params_from_csv = SeriesParamLoader.from_csv
load_series_params_from_data = SeriesParamLoader.from_data
load_accel_params_from_data = AccelParamLoader.from_data
