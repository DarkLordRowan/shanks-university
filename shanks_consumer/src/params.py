from enum import Enum
from dataclasses import dataclass
import pathlib
import json
import pyshanks
from abc import ABC, abstractmethod
from typing import List


class SeriesType(str, Enum):
    SYNTHETIC = "synthetic"
    NATURAL = "natural"


class BaseSeriesParam(ABC):

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> List[int] | None: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[pyshanks.SeriesBase]: ...

    @property
    @abstractmethod
    def series_type(self) -> SeriesType: ...


@dataclass
class SeriesParamJSON(BaseSeriesParam):
    name: str
    x: List[int] | None = None
    location: str | None = None

    @property
    def arguments(self):
        return self.x

    @property
    def series_name(self):
        return self.name

    @property
    def executable(self):
        return getattr(pyshanks, self.name)

    @property
    def series_type(self):
        return (
            SeriesType.NATURAL
            if self.location is not None
            else SeriesType.SYNTHETIC
        )


@dataclass
class SeriesParamModule(BaseSeriesParam):
    caller: type[pyshanks.SeriesBase]
    x: List[int] | None = None

    @property
    def arguments(self):
        return self.x

    @property
    def series_name(self):
        return self.caller.__name__

    @property
    def executable(self):
        return self.caller

    @property
    def series_type(self):
        return SeriesType.SYNTHETIC


class BaseAccelParam(ABC):

    @property
    @abstractmethod
    def accel_name(self) -> str: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[pyshanks.SeriesAcceleration]: ...

    @property
    @abstractmethod
    def n_values(self) -> List[int]: ...

    @property
    @abstractmethod
    def m_values(self) -> List[int]: ...

    @property
    @abstractmethod
    def additional_args(self) -> dict: ...


@dataclass
class StandardAccelParam(BaseAccelParam):
    n: List[int]
    m: List[int]

    @property
    def n_values(self):
        return self.n

    @property
    def m_values(self):
        return self.m


@dataclass
class AccelParamJSON(StandardAccelParam):
    name: str
    n: List[int]
    m: List[int]
    init_args: dict | None = None

    def __post_init__(self):
        self.expanded_init_args = {}
        if self.init_args:
            for key, value in self.init_args.items():
                if isinstance(value, dict):
                    self.expanded_init_args[key] = getattr(
                        pyshanks, value["type"]
                    )(value["value"])
                else:
                    self.expanded_init_args[key] = value

    @property
    def accel_name(self):
        return self.name

    @property
    def executable(self):
        return getattr(pyshanks, self.name)

    @property
    def additional_args(self):
        return self.expanded_init_args


@dataclass
class AccelParamModule(StandardAccelParam):
    def __init__(
        self,
        caller: type[pyshanks.SeriesAcceleration],
        n: List[int],
        m: List[int],
        **kwargs,
    ):
        self.caller = caller
        self.init_args = kwargs

        super().__init__(n, m)

    @property
    def accel_name(self):
        return self.caller.__name__

    @property
    def executable(self):
        return self.caller

    @property
    def additional_args(self):
        return self.init_args or {}


@dataclass
class TrialParameters:
    series: List[SeriesParamJSON]
    methods: List[AccelParamJSON]


def get_trial_params(json_location: pathlib.Path) -> TrialParameters | None:
    with open(json_location, encoding="utf-8") as f:
        data = json.load(f)

    series_list = []
    for series_data in data["series"]:
        x_value = series_data.get("x", None)
        if x_value is not None and not isinstance(x_value, list):
            x_value = [x_value]

        series_list.append(
            SeriesParamJSON(
                series_data["name"],
                x=x_value,
                location=series_data.get("location"),
            )
        )

    methods_list = []
    for method_data in data["methods"]:
        n_value = method_data["n"]
        m_value = method_data["m"]

        if not isinstance(n_value, list):
            n_value = [n_value]
        if not isinstance(m_value, list):
            m_value = [m_value]

        methods_list.append(
            AccelParamJSON(
                name=method_data["name"],
                n=n_value,
                m=m_value,
                init_args=method_data.get("args", {}),
            )
        )

    return TrialParameters(series=series_list, methods=methods_list)
