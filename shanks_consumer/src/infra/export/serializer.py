from dataclasses import asdict, is_dataclass
from typing import Any, Sequence

from tqdm import tqdm

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

    def _apply_view(self, result: TrialResult) -> dict:
        view_map = {
            ResultViewKind.FULL: self._view_full,
            ResultViewKind.SHORT: self._view_short,
            ResultViewKind.COMPLETION: self._view_completion_only,
            ResultViewKind.ERRORS_ONLY: self._view_errors_only,
        }

        viewer = view_map.get(self.view, self._view_full)
        return viewer(result)

    @staticmethod
    def _view_full(result: TrialResult) -> dict:
        return asdict(result)

    @staticmethod
    def _view_short(result: TrialResult) -> dict:
        raw = asdict(result)
        raw.pop("computed", None)
        return raw

    @staticmethod
    def _view_errors_only(result: TrialResult) -> dict | None:
        raw = asdict(result)
        return (
            {
                "series": result.series,
                "accel": result.accel,
                "error": raw.get("error"),
            }
            if raw.get("error")
            else None
        )

    @staticmethod
    def _view_completion_only(result: TrialResult) -> dict:
        return {
            "series": f"{result.series.name}_{list(result.series.arguments.items())}",
            "accel": f"{result.accel.name}_{result.accel.m_value}_{list(result.accel.additional_args.items())}",
            "failed": result.failed_iteration,
        }

    def to_dict(
        self,
        results: Sequence[TrialResult],
    ) -> Sequence[dict]:
        return list(
            filter(
                lambda d: d is not None,
                [
                    self._sanitize_value(self._apply_view(r))
                    for r in tqdm(results, "Serializing results...")
                ],
            )
        )
