from typing import Any, Iterable, Mapping

import pyshanks as ps

from src.domain.application.param_processing import autowrap
from src.domain.params import AccelParamJSON
from src.domain.precision import PrecisionType, cast_real_subtype_value
from src.domain.sources import AccelParamSource


class DataAccelParamSource(AccelParamSource):
    def __init__(self, data: Mapping[str, Any]):
        self.data = data

    def load(self, precision: PrecisionType) -> Iterable[AccelParamJSON]:
        methods = []

        for m in self.data["methods"]:
            n_list = [int(v) for v in autowrap(m["n"])]
            m_list = [int(v) for v in autowrap(m["m"])]

            args = self._convert_args(m.get("args", {}), precision)

            methods.append(
                AccelParamJSON(
                    name=m["name"] + precision.value,
                    n=n_list,
                    m=m_list,
                    init_args=args,
                    precision=precision,
                )
            )
        return methods

    def _convert_args(self, raw, precision: PrecisionType):
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
