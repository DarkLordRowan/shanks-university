"""
Parameter configuration module for series acceleration experiments.

Provides helpers for loading/normalising series and acceleration parameters
for every precision exported by ``py.cpp`` (F32, F64, FLong, Arb, CF32, CF64, CFLong, CArb).

Author: Yadrentsev I. M.
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

SeriesPregenLocalValue = tuple[SeriesResultProto[NumericLike], NumericLike]

SeriesPregenLocalKey = tuple[tuple[str, Any], ...]

SeriesPregenLocalMapping = dict[
    SeriesPregenLocalKey,
    SeriesPregenLocalValue | None,
]


def argument_combos[T](
    arguments: Mapping[str, Iterable[T]],
) -> Generator[Mapping[str, T], None, None]:
    """Generate all combinations of arguments.

    :param arguments: _argument mapping
    :type arguments: Mapping[str, Iterable[T]]
    :return: _generator of argument combinations
    :rtype: Generator[Mapping[str, T], None, None]
    """
    items = arguments.items()
    keys = [name for name, _ in items]
    values = [list(values) for _, values in items]
    return (
        dict(zip(keys, argument_combo)) for argument_combo in itertools.product(*values)
    )


@dataclass
class BaseSeriesParam[T]:
    """Abstract base class for series parameter configurations."""

    precision: PrecisionType
    _next_id: int = field(
        init=False,
        repr=False,
        default=1,
        metadata={"skip_dataclass_field": True},
    )
    id: int = field(init=False)
    __pregen: SeriesPregenLocalMapping = field(init=False, default_factory=dict)

    def __post_init__(self):
        self.id = BaseSeriesParam._next_id
        BaseSeriesParam._next_id += 1

    @property
    @abstractmethod
    def series_name(self) -> str:
        """Abstract property for the series name."""

    @property
    @abstractmethod
    def arguments(self) -> Mapping[str, Iterable[T]]:
        """Abstract property for the series arguments."""

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[SeriesBaseProto[TNum]] | Callable[..., SeriesResultProto[TNum]]:
        """Abstract property for the series executable."""

    @property
    def argument_combos(
        self,
    ) -> Generator[Mapping[str, T], None, None]:
        """Generate all combinations of series arguments."""
        return argument_combos(self.arguments)

    @staticmethod
    def _is_series_generator(
        candidate: object,
    ) -> TypeGuard[SeriesBaseProto[Any]]:
        """Inspect if the candidate is a valid series generator."""
        return hasattr(candidate, "generateSeries") and hasattr(candidate, "get_sum")

    @property
    def instance(self) -> SeriesBaseProto[NumericLike]:
        """Get an instance of the series generator."""
        instance = self.executable()
        if not self._is_series_generator(instance):
            raise TypeError(
                f"Series executable '{self.series_name}' did not return a valid generator"
            )
        return instance

    def __resolve_series_arguments(
        self,
        argument: Mapping[str, Any],
        size_floor: int,
    ) -> tuple[Any, int, Any, int]:
        """Resolve series arguments from the provided mapping.

        If certain arguments are missing, default values are used.

        :param argument: _series argument mapping
        :type argument: Mapping[str, Any]
        :param size_floor: _minimum size for the series
        :type size_floor: int
        :return: _resolved arguments (x, vecSize, addTParameter, addKParameter)
        :rtype: tuple[Any, int, Any, int]
        """
        vec_size = int(argument.get("vecSize", size_floor))
        vec_size = max(vec_size, size_floor)

        default_t = cast_precision_value(self.precision, 1)
        add_t_value = argument.get("addTParameter", argument.get("a", default_t))

        add_k_source = argument.get(
            "addKParameter", argument.get("m", argument.get("b", 1))
        )
        add_k_value = int(add_k_source) if add_k_source is not None else 1

        default_x = cast_precision_value(self.precision, 0)
        x_value = argument.get("x", default_x)

        return x_value, vec_size, add_t_value, add_k_value

    def __get_from_pregen(
        self, argument: Mapping[str, Any]
    ) -> SeriesPregenLocalValue | None:
        """Get a pre-generated series result from the cache."""
        if not self.__pregen:
            return None
        return self.__pregen.get(tuple(argument.items()), None)

    def __generate(
        self, x: Any, vec_size: int, t: Any, k: int
    ) -> SeriesPregenLocalValue:
        """Generate a series result using the series instance."""
        instance = self.instance
        return (
            instance.generateSeries(x, vec_size, t, k),
            instance.get_sum(),
        )

    def obtain_by_argument(
        self, argument: Mapping[str, Any], size_floor: int
    ) -> SeriesPregenLocalValue:
        """Obtain a series result for the given argument mapping.

          If a pre-generated result exists, it is returned from the cache. Otherwise,
          a new result is generated and cached.

        :param argument: _series argument mapping
          :type argument: Mapping[str, Any]
          :param size_floor: _minimum size for the series
          :type size_floor: int
          :return: _series result and its limit
          :rtype: SeriesPregenLocalValue
        """
        series_result_lim = self.__get_from_pregen(argument)

        if series_result_lim:
            return series_result_lim

        x, vec_size, t, k = self.__resolve_series_arguments(argument, size_floor)

        series_result_lim = self.__generate(x, vec_size, t, k)

        self.__pregen[tuple(argument.items())] = series_result_lim
        return series_result_lim

    def obtain_all(self, size_floor: int) -> SeriesPregenLocalMapping:
        """Obtain all pre-generated series results for all argument combinations.

        :param size_floor: _minimum size for the series
        :type size_floor: int
        :return: _mapping of argument combinations to series results
        :rtype: SeriesPregenLocalMapping
        """
        return {
            tuple(argument.items()): self.obtain_by_argument(argument, size_floor)
            for argument in self.argument_combos
        }

    @property
    def pregen(self):
        """Get the pre-generated series results mapping."""
        return self.__pregen


@dataclass
class SeriesParamJSON(BaseSeriesParam[TNum]):
    """Series parameter configuration loaded from JSON.

    :param BaseSeriesParam: _base series parameter class
    :type BaseSeriesParam: _dataclass_
    :return: _series parameter configuration
    :rtype: _dataclass_
    """

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
    """Series parameter configuration loaded from a module.

    :param BaseSeriesParam: _base series parameter class
    :type BaseSeriesParam: _dataclass_
    :return: _series parameter configuration
    :rtype: _dataclass_
    """

    caller: type[SeriesBaseProto[TNum]]
    args: Mapping[str, Iterable[TNum]]

    def __init__(
        self,
        caller: type[SeriesBaseProto[TNum]],
        **kwargs,
    ):
        self.caller = caller
        self.args = kwargs
        super().__init__(precision=PrecisionType.F64)

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
    """Series parameter configuration loaded from a CSV file.

    :param BaseSeriesParam: _base series parameter class
    :type BaseSeriesParam: _dataclass_
    :return: _series parameter configuration
    :rtype: _dataclass_
    """

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
    """Wrapper for CSV-based series generators.

    :param Generic: _generic type_
    :type Generic: _type_
    """

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
    """Event specifier for acceleration parameters."""

    type: EventType
    log_action_capacity: int | None = None
    stop_action_limit: int | None = None


class BaseAccelParam[T]:
    """Abstract base class for acceleration parameter configurations."""

    precision: PrecisionType
    events: Iterable[EventSpecifierParam]

    @property
    @abstractmethod
    def accel_name(self) -> str:
        """Abstract property for the acceleration method name."""

    @property
    @abstractmethod
    def executable(
        self,
    ) -> type[AccelProto[TNum]]:  # type: ignore
        """Abstract property for the acceleration method executable."""

    @property
    def size_floor(self) -> int:
        """Calculate the minimum required size for the series."""
        return max(10, max(self.n_values) + max(self.m_values) + 5)

    def create_instance(self, args: Mapping[str, Any]) -> AccelProto[NumericLike]:
        """Create an instance of the acceleration method with the given arguments."""
        return self.executable(**args)

    @property
    @abstractmethod
    def n_values(self) -> Iterable[int]:
        """Abstract property for the n values used in acceleration."""

    @property
    @abstractmethod
    def m_values(self) -> Iterable[int]:
        """Abstract property for the m values used in acceleration."""

    @property
    @abstractmethod
    def additional_args(self) -> dict[str, Iterable[Any]]:
        """Abstract property for additional arguments for the acceleration method."""

    def display_args(self, args: Mapping[str, Any]) -> Mapping[str, str]:
        """Display the additional arguments as strings."""
        return {key: str(value) for key, value in args.items()}

    @property
    def argument_combos(
        self,
    ) -> Generator[Mapping[str, T], None, None]:
        """Argument combinations generator for additional arguments."""
        return argument_combos(self.additional_args)

    def create_event_context(self):
        """Event processing context initializer."""
        return {
            "counters": {e.type: 0 for e in self.events},
            "stopped": {e.type: False for e in self.events},
            "blocked": False,
        }

    def process_events(self, computed: list, ctx: dict[str, Any]) -> list:
        """Process events based on the current context and computed results."""
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
    """Standard acceleration parameter configuration."""

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
    """Acceleration parameter configuration loaded from JSON."""

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
    """Acceleration parameter configuration loaded from a module."""

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
