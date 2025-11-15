import pandas as pd

import src.viz.stages as stages


def apply_pipeline(collection, pipeline: list[stages.PipelineStage]) -> pd.DataFrame:
    data = list(collection.aggregate([stage.build() for stage in pipeline]))

    if data and "_id" in data[0].keys():
        processed_data = [
            {
                **item["_id"],
                **{k: v for k, v in item.items() if k != "_id"},
            }
            for item in data
        ]
    else:
        processed_data = data

    return pd.DataFrame(processed_data)


def accel_error_events_pipeline(
    stack_id: str | None = None,
) -> list[stages.PipelineStage]:
    return [
        stages.FilterByStackIdStage(stack_id),
        stages.GroupByMethodStage(),
        stages.ErrorRateStage(),
        stages.EventsUnwindStage(),
        stages.EventsUnwindStage(),
        stages.EventGroupingStage(),
        stages.StatsGroupingStage(),
        stages.SortErrorRateStage(),
        stages.NormalizeAdditionalArgsStage(),
    ]


def computed_values_pipeline(
    series_name: str, method_name: str, stack_id: str | None = None
) -> list[stages.PipelineStage]:
    return [
        stages.FilterByStackIdStage(stack_id),
        stages.FilterByMethodSeriesStage(method_name, series_name),
        stages.UnwindComputedStage(),
        stages.FlattenComputedDataStage(),
    ]


def unqiue_fields_pipeline(
    field_path: str, stack_id: str | None = None
) -> list[stages.PipelineStage]:
    return [
        stages.FilterByStackIdStage(stack_id),
        stages.GroupByFieldStage(field_path),
        stages.ProjectFieldStage(field_path),
        stages.SortFieldStage(),
    ]
