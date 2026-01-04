"""
Data acceleration parameter source implementation.
Author: Yadrentsev I. M.
"""

from typing import Any, Iterable, Mapping

import pyshanks as ps
from src.domain.application.param_processing import autowrap
from src.domain.params import AccelParamJSON, EventSpecifierParam
from src.domain.precision import PrecisionType, cast_real_subtype_value
from src.domain.sources import AccelParamSource


class DataAccelParamSource(AccelParamSource):
    def __init__(self, data: Mapping[str, Any]):
        self.data = data

    def load(self, precision: PrecisionType) -> Iterable[AccelParamJSON]:
        """Loads acceleration parameters from provided data.
        :param precision: The precision type for parameter conversion.
        :type precision: PrecisionType
        :return: An iterable of AccelParamJSON instances.
        :rtype: Iterable[AccelParamJSON]"""
        methods = []

        for m in self.data["methods"]:
            n_list = [int(v) for v in autowrap(m["n"])]
            m_list = [int(v) for v in autowrap(m["m"])]

            args = self._convert_args(m.get("args", {}), precision)
            events = [EventSpecifierParam(**event) for event in m.get("events", [])]

            methods.append(
                AccelParamJSON(
                    name=m["name"] + precision.value,
                    n=n_list,
                    m=m_list,
                    init_args=args,
                    precision=precision,
                    events=events,
                )
            )
        return methods

    def _convert_args(self, raw, precision: PrecisionType):
        """Converts raw argument values to appropriate types based on precision.

        The run values are processed as follows:
        - If the key is "remainder", the values are converted to ps.RemainderType enums.
        - If the key is "numerator", the values are converted to ps.NumeratorType enums.
        - For other keys, the values are converted to the appropriate real subtype based on the given precision.

        :param raw: The raw argument mapping.
        :type raw: Mapping[str, Any]
        :param precision: The precision type for value conversion.
        :type precision: PrecisionType
        :return: A mapping of converted argument values.
        :rtype: Mapping[str, list[Any]]
        """
        args = {}
        for key, value in raw.items():
            wrapped = autowrap(value)
            if key == "remainder":
                args[key] = [getattr(ps.RemainderType, v) for v in wrapped]  # type: ignore
            elif key == "numerator":
                args[key] = [getattr(ps.NumeratorType, v) for v in wrapped]  # type: ignore
            else:
                args[key] = [cast_real_subtype_value(precision, v) for v in wrapped]
        return args
