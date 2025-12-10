"""
Parameter configuration module for series acceleration experiments.

Provides helpers for loading/normalising series and acceleration parameters
for every precision exported by ``py.cpp`` (F32, F64, FLong, Arb, CF32, CF64, CFLong, CArb).
"""

import pathlib
import itertools
from abc import ABC, abstractmethod
from collections.abc import Callable, Iterable, Sequence
from dataclasses import dataclass, field
from typing import Any, Generic, Generator, Mapping, override, TypeGuard

import pyshanks as ps
from src.domain.event import EventType, EVENT_METHODS
from src.domain.precision import (
    AccelProto,
    PrecisionType,
    SeriesBaseProto,
    SeriesResultProto,
    NumericLike,
    TNum,
    cast_natural_series_value,
    create_series_result,
    cast_precision_value,
)


def argument_combos[T](
    arguments: Mapping[str, Iterable[T]],
) -> Generator[Mapping[str, T], None, None]:
    items = arguments.items()
    keys = [name for name, _ in items]
    values = [list(values) for _, values in items]
    return (
        dict(zip(keys, argument_combo))
        for argument_combo in itertools.product(*values)
    )


@dataclass
class BaseSeriesParam[T]:
    """Abstract base class for series parameter configurations."""

    precision: PrecisionType

    @property
    @abstractmethod
    def series_name(self) -> str: ...

    @property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[T]]: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> (
        type[SeriesBaseProto[TNum]] | Callable[..., SeriesResultProto[TNum]]
    ): ...

    @property
    def argument_combos(
        self,
    ) -> Generator[Mapping[str, T], None, None]:
        return argument_combos(self.arguments)

    @staticmethod
    def _is_series_generator(
        candidate: object,
    ) -> TypeGuard[SeriesBaseProto[Any]]:
        return hasattr(candidate, "generateSeries") and hasattr(
            candidate, "get_sum"
        )

    @property
    def instance(self) -> SeriesBaseProto[NumericLike]:
        instance = self.executable()
        if not self._is_series_generator(instance):
            raise TypeError(
                f"Series executable '{self.series_name}' did not return a valid generator"
            )
        return instance

    def resolve_series_arguments(
        self,
        argument: Mapping[str, Any],
        size_floor: int,
    ) -> tuple[Any, int, Any, int]:
        vec_size = int(argument.get("vecSize", size_floor))
        vec_size = max(vec_size, size_floor)

        default_t = cast_precision_value(self.precision, 1)
        add_t_value = argument.get(
            "addTParameter", argument.get("a", default_t)
        )

        add_k_source = argument.get(
            "addKParameter", argument.get("m", argument.get("b", 1))
        )
        add_k_value = int(add_k_source) if add_k_source is not None else 1

        default_x = cast_precision_value(self.precision, 0)
        x_value = argument.get("x", default_x)

        return x_value, vec_size, add_t_value, add_k_value

    def generate(
        self, x: Any, vec_size: int, t: Any, k: int
    ) -> tuple[SeriesResultProto[NumericLike], NumericLike]:
        instance = self.instance
        return (
            instance.generateSeries(x, vec_size, t, k),
            instance.get_sum(),
        )

    def execute(
        self, argument: Mapping[str, Any], size_floor: int
    ) -> tuple[SeriesResultProto[NumericLike], NumericLike]:
        x, vec_size, t, k = self.resolve_series_arguments(argument, size_floor)
        series_result, lim = self.generate(x, vec_size, t, k)
        return series_result, lim


@dataclass
class SeriesParamJSON(BaseSeriesParam[TNum]):
    name: str
    args: Mapping[str, Iterable[TNum]]

    @property
    def arguments(self):
        return self.args

    @property
    def series_name(self):
        return self.name

    @property
    def executable(self):
        return getattr(ps, self.name)


@dataclass
class SeriesParamModule(BaseSeriesParam[TNum]):
    caller: type[SeriesBaseProto[TNum]]
    args: Mapping[str, Iterable[TNum]]

    def __init__(
        self,
        caller: type[SeriesBaseProto[TNum]],
        **kwargs,
    ):
        self.caller = caller
        self.args = kwargs
        super().__init__(
            precision=PrecisionType.F64
        )  # ! need to obtain precision from caller instead

    @property
    def arguments(self):
        return self.args

    @property
    def series_name(self):
        return self.caller.__name__

    @property
    def executable(self):
        return self.caller


@dataclass
class SeriesParamCSV(BaseSeriesParam[TNum]):
    location: pathlib.Path
    row: int
    raw_values: tuple[str, ...]

    @property
    def arguments(self):
        return {}

    @property
    def series_name(self):
        return f"{self.location.name}#{self.row}"

    @property
    @override
    def executable(self):  # type: ignore
        def factory():
            return CSVSeriesWrapper(self.precision, self.raw_values)

        return factory


class CSVSeriesWrapper(Generic[TNum]):
    def __init__(self, precision: PrecisionType, raw_values: Sequence[str]):
        values = [cast_natural_series_value(precision, value) for value in raw_values]
        self.data = create_series_result(values, precision)
        self._sum = self.data.Sn[-1]
        self._precision = precision

    def generateSeries(
        self,
        x: TNum,
        vecSize: int,
        addTParameter: TNum,
        addKParameter: int,
    ) -> SeriesResultProto[TNum]:
        return self.data

    def get_sum(self) -> TNum:
        return self._sum

    def get_name(self) -> str:
        return f"CSVSeries_{self._precision.value}"


@dataclass
class EventSpecifierParam:
    type: EventType
    log_action_capacity: int | None = None
    stop_action_limit: int | None = None


class BaseAccelParam[T]:
    precision: PrecisionType
    events: Iterable[EventSpecifierParam]

    @property
    @abstractmethod
    def accel_name(self) -> str: ...

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[AccelProto[TNum]]: ...  # type: ignore

    @property
    def size_floor(self) -> int:
        return max(10, max(self.n_values) + max(self.m_values) + 5)

    def create_instance(
        self, args: Mapping[str, Any]
    ) -> AccelProto[NumericLike]:
        return self.executable(**args)

    @property
    @abstractmethod
    def n_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def m_values(self) -> Iterable[int]: ...

    @property
    @abstractmethod
    def additional_args(self) -> dict[str, Iterable[Any]]: ...

    def display_args(self, args: Mapping[str, Any]) -> Mapping[str, str]:
        return {key: str(value) for key, value in args.items()}

    @property
    def argument_combos(
        self,
    ) -> Generator[Mapping[str, T], None, None]:
        return argument_combos(self.additional_args)

    def create_event_context(self):
        return {
            "counters": {e.type: 0 for e in self.events},
            "stopped": {e.type: False for e in self.events},
            "blocked": False,
        }

    def process_events(self, computed: list, ctx: dict[str, Any]) -> list:
        events_here = []

        for e in self.events:
            if ctx["stopped"][e.type]:
                continue

            handler = EVENT_METHODS[EventType(e.type)]
            result = handler(computed)

            if result is not None:
                events_here.append(result)
                ctx["counters"][e.type] += 1

                if (
                    e.log_action_capacity
                    and ctx["counters"][e.type] >= e.log_action_capacity
                ):
                    ctx["stopped"][e.type] = True

                if (
                    e.stop_action_limit
                    and ctx["counters"][e.type] >= e.stop_action_limit
                ):
                    ctx["blocked"] = True

        return events_here


@dataclass
class StandardAccelParam(BaseAccelParam[TNum], ABC):
    n: Iterable[int]
    m: Iterable[int]
    events: Iterable[EventSpecifierParam]
    precision: PrecisionType

    @property
    @override
    def n_values(self):
        return self.n

    @property
    @override
    def m_values(self):
        return self.m


@dataclass
class AccelParamJSON(StandardAccelParam[TNum]):
    name: str
    init_args: Mapping[str, Iterable[Any]]

    def __post_init__(self):
        self.expanded_init_args = {
            key: list(value) for key, value in (self.init_args or {}).items()
        }

    @property
    @override
    def accel_name(self):
        return self.name

    @property
    @override
    def executable(self):
        return getattr(ps, self.accel_name)

    @property
    @override
    def additional_args(self):  # type: ignore
        return self.expanded_init_args


class AccelParamModule(StandardAccelParam[TNum]):

    def __init__(
        self,
        caller: type[AccelProto[TNum]],
        n: Iterable[int],
        m: Iterable[int],
        events: Iterable[EventSpecifierParam],
        **kwargs,
    ):
        self.caller = caller
        self.init_args = kwargs
        super().__init__(n, m, events)

    @property
    @override
    def accel_name(self):
        return self.caller.__name__

    @property
    @override
    def executable(self):
        return self.caller

    @property
    @override
    def additional_args(self):
        return self.init_args or {}
