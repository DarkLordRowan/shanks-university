from abc import ABC, abstractmethod


class PipelineStage(ABC):
    @abstractmethod
    def build(self) -> dict:
        pass


class MatchStage(PipelineStage):
    def _build_match_expression(self) -> dict:
        raise NotImplementedError(
            "Subclasses must implement _build_match_expression"
        )

    def build(self) -> dict:
        return {"$match": self._build_match_expression()}


class GroupStage(PipelineStage):
    def _build_group_expression(self) -> dict:
        raise NotImplementedError(
            "Subclasses must implement _build_group_expression"
        )

    def build(self) -> dict:
        return {"$group": self._build_group_expression()}


class AddFieldsStage(PipelineStage):
    def _build_add_fields_expression(self) -> dict:
        raise NotImplementedError(
            "Subclasses must implement _build_add_fields_expression"
        )

    def build(self) -> dict:
        return {"$addFields": self._build_add_fields_expression()}


class ProjectStage(PipelineStage):
    def _build_project_expression(self) -> dict:
        raise NotImplementedError(
            "Subclasses must implement _build_project_expression"
        )

    def build(self) -> dict:
        return {"$project": self._build_project_expression()}


class UnwindStage(PipelineStage):
    def _build_unwind_expression(self) -> dict | str:
        raise NotImplementedError(
            "Subclasses must implement _build_unwind_expression"
        )

    def build(self) -> dict:
        return {"$unwind": self._build_unwind_expression()}


class SortStage(PipelineStage):
    def _build_sort_expression(self) -> dict:
        raise NotImplementedError(
            "Subclasses must implement _build_sort_expression"
        )

    def build(self) -> dict:
        return {"$sort": self._build_sort_expression()}


class NormalizeAdditionalArgsStage(AddFieldsStage):
    def _build_add_fields_expression(self) -> dict:
        return {
            "accel.additional_args": {
                "$cond": [
                    {
                        "$or": [
                            {"$eq": ["$accel.additional_args", None]},
                            {"$eq": ["$accel.additional_args", {}]},
                            {"$not": ["$accel.additional_args"]},
                        ]
                    },
                    {"_empty": True},
                    "$accel.additional_args",
                ]
            }
        }


class GroupByMethodStage(GroupStage):
    def _build_group_expression(self) -> dict:
        return {
            "_id": {
                "method_name": "$accel.name",
                "m_value": "$accel.m_value",
                "additional_args": "$accel.additional_args",
            },
            "total": {"$sum": 1},
            "errors": {
                "$sum": {"$cond": [{"$ifNull": ["$error", False]}, 1, 0]}
            },
            "events_list": {"$push": "$events"},
        }


class ErrorRateStage(AddFieldsStage):
    def _build_add_fields_expression(self) -> dict:
        return {"error_rate": {"$divide": ["$errors", "$total"]}}


class EventsUnwindStage(UnwindStage):
    def _build_unwind_expression(self) -> dict:
        return {
            "path": "$events_list",
            "preserveNullAndEmptyArrays": True,
        }


class EventGroupingStage(GroupStage):
    def _build_group_expression(self) -> dict:
        return {
            "_id": {
                "method_name": "$_id.method_name",
                "m_value": "$_id.m_value",
                "additional_args": "$_id.additional_args",
                "event_type": "$events_list.event",
            },
            "error_rate": {"$first": "$error_rate"},
            "count": {"$sum": 1},
            "total_docs": {"$first": "$total"},
            "total_errors": {"$first": "$errors"},
        }


class StatsGroupingStage(GroupStage):
    def _build_group_expression(self) -> dict:
        return {
            "_id": {
                "method_name": "$_id.method_name",
                "m_value": "$_id.m_value",
                "additional_args": "$_id.additional_args",
            },
            "error_rate": {"$first": "$error_rate"},
            "total_docs": {"$first": "$total_docs"},
            "total_errors": {"$first": "$total_errors"},
            "events": {
                "$push": {
                    "event_type": "$_id.event_type",
                    "count": "$count",
                }
            },
            "total_events": {"$sum": "$count"},
        }


class SortErrorRateStage(SortStage):
    def _build_sort_expression(self) -> dict:
        return {"error_rate": 1}


class FilterByStackIdStage(MatchStage):
    def __init__(self, stack_id: str | None = None):
        self.stack_id = stack_id

    def _build_match_expression(self) -> dict:
        if self.stack_id:
            return {"stack_id": self.stack_id}
        return {}


class FilterByMethodSeriesStage(MatchStage):
    def __init__(self, method_name: str, series_name: str):
        self.method_name = method_name
        self.series_name = series_name

    def _build_match_expression(self) -> dict:
        return {
            "accel.name": self.method_name,
            "series.name": self.series_name,
        }


class UnwindComputedStage(UnwindStage):
    def _build_unwind_expression(self) -> str:
        return "$computed"


class FlattenComputedDataStage(ProjectStage):
    def _build_project_expression(self) -> dict:
        return {
            "_id": 0,
            "n": "$computed.n",
            "m": "$accel.m_value",
            "series_name": "$series.name",
            "stack_id": "$stack_id",
            "lim": "$series.lim",
            "partial_sum": "$computed.partial_sum",
            "accel_value": "$computed.accel_value",
            "partial_sum_deviation": "$computed.partial_sum_deviation",
            "accel_value_deviation": "$computed.accel_value_deviation",
            "additional_args": "$accel.additional_args",
        }


class GroupByFieldStage(GroupStage):
    def __init__(self, field_path: str):
        self.field_path = field_path

    def _build_group_expression(self) -> dict:
        return {"_id": f"${self.field_path}"}


class ProjectFieldStage(ProjectStage):
    def __init__(self, field_path: str):
        self.field_path = field_path
        self.field_name = (
            field_path.split(".")[-1] if "." in field_path else field_path
        )

    def _build_project_expression(self) -> dict:
        return {"_id": 0, self.field_name: "$_id"}


class SortFieldStage(SortStage):
    def _build_sort_expression(self) -> dict:
        return {"_id": 1}
