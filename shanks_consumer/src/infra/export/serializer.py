from dataclasses import asdict, is_dataclass
from typing import Any, Sequence

import pyshanks as ps
from tqdm import tqdm

from src.domain.trial_result import TrialResult


class TrialResultSerializer:
    @staticmethod
    def sanitize_value(value: Any) -> Any:
        if isinstance(value, (ps.Arb, ps.CArb, ps.CF32, ps.CF64, ps.CFLong, float)):
            return str(value)
        if isinstance(value, (ps.RemainderType, ps.NumeratorType)):
            return value.name
        if is_dataclass(value):
            return {
                k: TrialResultSerializer.sanitize_value(v)
                for k, v in asdict(value).items()  # type: ignore
            }
        if isinstance(value, dict):
            return {
                k: TrialResultSerializer.sanitize_value(v) for k, v in value.items()
            }
        if isinstance(value, list):
            return [TrialResultSerializer.sanitize_value(v) for v in value]
        return value

    @staticmethod
    def to_dict(results: Sequence[TrialResult]) -> list[dict]:
        return [
            TrialResultSerializer.sanitize_value(asdict(r))
            for r in tqdm(results, "Sanitazing values...")
        ]
