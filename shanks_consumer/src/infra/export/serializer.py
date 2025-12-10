from dataclasses import asdict, is_dataclass
from typing import Any, Sequence

import pyshanks as ps
from src.domain.application.result_view import ResultViewKind
from src.domain.data_serializer import DataSerializer
from src.domain.trial_result import TrialResult


class TrialResultSerializer(DataSerializer):

    def __init__(self, view: ResultViewKind = ResultViewKind.FULL):
        self.view = view

    def _sanitize_value(self, value: Any) -> Any:
        if isinstance(value, (ps.Arb, ps.CArb, ps.CF32, ps.CF64, ps.CFLong, float)):
            return str(value)
        if isinstance(value, (ps.RemainderType, ps.NumeratorType)):
            return value.name
        if is_dataclass(value):
            return {
                k: self._sanitize_value(v)
                for k, v in asdict(value).items()  # type: ignore
            }
        if isinstance(value, dict):
            return {k: self._sanitize_value(v) for k, v in value.items()}
        if isinstance(value, list):
            return [self._sanitize_value(v) for v in value]
        return value

    def to_dict(
        self,
        results: Sequence[TrialResult],
    ) -> Sequence[dict]:
        return [self._sanitize_value(r) for r in results]
