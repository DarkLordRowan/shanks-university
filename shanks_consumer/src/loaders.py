import csv
import json
import pathlib
from typing import Any, Iterable, Iterator

import pyshanks as ps
from src.params import AccelParamJSON, SeriesParamCSV, SeriesParamJSON


class JSONConfigHandler:
    class ArbEncoder(json.JSONEncoder):
        def default(self, o):
            if isinstance(o, ps.Arb):
                return str(o)
            if isinstance(o, pathlib.Path):
                return str(o)
            return super().default(o)

    class ArbDecoder(json.JSONDecoder):
        def __init__(self, *args, **kwargs):
            super().__init__(
                parse_float=self._parse_number,
                parse_int=self._parse_number,
                *args,
                **kwargs,
            )

        @staticmethod
        def _parse_number(value):
            return ps.Arb(str(value))

    @classmethod
    def load_config(cls, json_location: pathlib.Path, arb: bool = False) -> dict:
        with open(json_location, encoding="utf-8") as f:
            return json.load(f, cls=cls.ArbDecoder) if arb else json.load(f)


class SeriesParamLoader:
    @staticmethod
    def from_json(
        json_location: pathlib.Path, arb: bool = False
    ) -> list[SeriesParamJSON]:
        data = JSONConfigHandler.load_config(json_location, arb)
        return SeriesParamLoader.from_data(data, arb)

    @staticmethod
    def from_data(data: dict, arb: bool = False) -> list[SeriesParamJSON]:
        return [
            SeriesParamLoader._create_series_param(series_data, arb)
            for series_data in data["series"]
        ]

    @staticmethod
    def _create_series_param(series_data: dict, arb: bool = False) -> SeriesParamJSON:
        name_suffix = "Arb" if arb else "F64"
        return SeriesParamJSON(
            name=series_data["name"] + name_suffix,
            args=SeriesParamLoader._process_series_args(series_data.get("args", {})),
        )

    @staticmethod
    def _process_series_args(args: Any) -> dict[str, Iterable[Any]]:
        if not isinstance(args, dict):
            return {"x": autowrap(args)}
        return {str(key): autowrap(value) for key, value in args.items()}

    @staticmethod
    def from_csv(
        csv_location: pathlib.Path, arb: bool = False
    ) -> Iterator[SeriesParamCSV]:
        with open(csv_location, encoding="utf-8") as f:
            for i, row in enumerate(csv.reader(f), 1):
                yield SeriesParamLoader._create_csv_series(
                    csv_location.name, i, row, arb
                )

    @staticmethod
    def _create_csv_series(
        source_name: str, row_num: int, row_data: list, arb: bool = False
    ) -> SeriesParamCSV:
        data: ps.ArraySeriesArb | ps.ArraySeriesF64 = (
            ps.ArraySeriesArb(row_data)
            if arb
            else ps.ArraySeriesF64(list(map(float, row_data)))
        )
        return SeriesParamCSV(source_name=source_name, row=row_num, data=data)


class AccelParamLoader:
    @staticmethod
    def from_json(
        json_location: pathlib.Path, arb: bool = False
    ) -> list[AccelParamJSON]:
        data = JSONConfigHandler.load_config(json_location, arb)
        return AccelParamLoader.from_data(data, arb)

    @staticmethod
    def from_data(data: dict, arb: bool = False) -> list[AccelParamJSON]:
        methods_list = []
        for method_data in data["methods"]:
            methods_list.append(AccelParamLoader._create_accel_param(method_data, arb))
        return methods_list

    @staticmethod
    def _create_accel_param(method_data: dict, arb: bool = False) -> AccelParamJSON:
        name_suffix = "Arb" if arb else "F64"
        init_args = {
            key: autowrap(value) for key, value in method_data.get("args", {}).items()
        }

        return AccelParamJSON(
            name=method_data["name"] + name_suffix,
            n=autowrap(method_data["n"]),
            m=autowrap(method_data["m"]),
            init_args=init_args,
        )


def autowrap(x: Any) -> Iterable[Any]:
    if x is not None and (isinstance(x, str) or not isinstance(x, Iterable)):
        return [x]
    if isinstance(x, dict) and all(k in x for k in ("start", "stop", "step")):
        return _generate_range_values(x["start"], x["stop"], x["step"])
    # ? assume x is iterable (JSON is limited on datatypes anyway)
    return x  # type: ignore


def _generate_range_values(start: float, stop: float, step: float) -> list[float]:
    if step == 0:
        raise ValueError("Step cannot be zero.")
    steps = int((stop - start) / step)
    return [start + i * step for i in range(steps)]


get_series_params_from_json = SeriesParamLoader.from_json
get_accel_params_from_json = AccelParamLoader.from_json
get_series_params_from_csv = SeriesParamLoader.from_csv
load_series_params_from_data = SeriesParamLoader.from_data
load_accel_params_from_data = AccelParamLoader.from_data

ArbEncoder = JSONConfigHandler.ArbEncoder
ArbDecoder = JSONConfigHandler.ArbDecoder
