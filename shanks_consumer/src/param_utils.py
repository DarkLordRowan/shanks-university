import csv
import json
import pathlib

from typing import Iterable, Any

import pyshanks as ps

from src.params import SeriesParamJSON, AccelParamJSON, SeriesParamCSV


class ArbEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, ps.Arb):
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
        """Convert a JSON numeric literal to a high‑precision FP instance."""
        return ps.Arb(str(value))


def get_series_params_from_json(
    json_location: pathlib.Path,
    arb: bool
) -> list[SeriesParamJSON]:
    """Load series parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.

    Returns:
        List of SeriesParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=ArbDecoder) if arb else json.load(f)
    return load_series_params_from_data(data, arb)


def load_series_params_from_data(
    data: dict,
    arb: bool,
) -> list[SeriesParamJSON]:
    series_list: list[SeriesParamJSON] = []
    for series_data in data["series"]:
        args = series_data.get("args", {})
        if not isinstance(args, dict):
            args = {"x": autowrap(args) }
        else:
            args = {
                str(key): autowrap(value)
                for key, value in args.items()
            }
        series_list.append(
            SeriesParamJSON(name=series_data.get("name")+("Arb" if arb else "F64"), args=args)
        )
    return series_list


def get_accel_params_from_json(
    json_location: pathlib.Path,
    arb: bool,
) -> list[AccelParamJSON]:
    """Load acceleration parameters from a JSON configuration file.

    Args:
        json_location: Path to the JSON configuration file.

    Returns:
        List of AccelParamJSON objects configured from the JSON data.

    Raises:
        FileNotFoundError: If the JSON file doesn't exist.
        JSONDecodeError: If the JSON file is malformed.
    """
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f, cls=ArbDecoder) if arb else json.load(f)
    return load_accel_params_from_data(data, arb)


def load_accel_params_from_data(
    data: dict,
    arb: bool,
) -> list[AccelParamJSON]:
    methods_list: list[AccelParamJSON] = []
    for method_data in data["methods"]:
        n_value = autowrap(method_data["n"])
        m_value = autowrap(method_data["m"])
        methods_list.append(
            AccelParamJSON(
                name=method_data["name"] + ("Arb" if arb else "F64"),
                n=n_value,
                m=m_value,
                init_args={key: autowrap(value) for key, value in method_data.get("args", {}).items()},
            )
        )
    return methods_list


def get_series_params_from_csv(
    csv_location: pathlib.Path,
    arb: bool
) -> Iterable[SeriesParamCSV]:
    """Load series parameters from a CSV file.

    Each row in the CSV file becomes a separate series parameter.

    Args:
        csv_location: Path to the CSV file.

    Returns:
        Iterable of SeriesParamCSV objects, one for each row in the CSV.

    Raises:
        FileNotFoundError: If the CSV file doesn't exist.
    """
    with open(csv_location, encoding="utf-8") as f:
        return [
            SeriesParamCSV(
                source_name=csv_location.name,
                row=i,
                data=ps.ArraySeriesArb(row) if arb else ps.ArraySeriesF64(
                    list(map(float, row))
                ),
            )
            for i, row in enumerate(csv.reader(f), 1)
        ]

def autowrap(x: Any) -> Iterable[Any]:
    if x is not None and (isinstance(x, str) or not isinstance(x, Iterable)):
        return [x]
    if isinstance(x, dict):
        # ? consider as range value
        return [
            x["start"] + i * x["step"]
            for i in range(int((x["stop"] - x["start"]) / x["step"]))
        ]
    return x

# def _is_concrete_subclass(cls: type, base: type) -> bool:
#     """
#     Return ``True`` if *cls* is a non‑abstract subclass of *base*.
#     """
#     return (
#         inspect.isclass(cls)
#         and issubclass(cls, base)
#         and cls is not base
#         and not inspect.isabstract(cls)
#     )


# def all_accel(
#     n: int | Iterable[int],
#     m: int | Iterable[int],
#     arb: bool,
#     extra_args: dict[str, Any] | None = None,
# ) -> list[BaseAccelParam]:
#     extra_args = extra_args or {}

#     accel_params: list[BaseAccelParam] = []

#     for _, cls in inspect.getmembers(ps, inspect.isclass):
#         if _is_concrete_subclass(cls, ps.SeriesAccelerationArb if arb else ps.SeriesAccelerationF64):
#             kwargs: dict[str, Any] = dict(extra_args.get(cls.__name__, {}))
#             accel_params.append(
#                 AccelParamModule(
#                     caller=cls,
#                     n=autowrap(n),
#                     m=autowrap(m),
#                     **kwargs,
#                 )
#             )
#     return accel_params