import logging
import pathlib

import pandas as pd
from pymongo.database import Database as MongoDatabase

from src.viz.pipeline import (accel_error_events_pipeline, apply_pipeline,
                              computed_values_pipeline, unqiue_fields_pipeline)
from src.viz.processor import DataProcessor
from src.viz.visualizer import DataVisualizer


class MongoDataCollector:
    def __init__(self, mongo_database: MongoDatabase, mongo_collection: str):
        self.db = mongo_database
        self.collection = mongo_database.get_collection(mongo_collection)
        self.data_processor = DataProcessor()
        self.visualizer = DataVisualizer()

    def get_error_and_event_stats_per_method(
        self,
        stack_id: str | None = None,
        draw_html: pathlib.Path | None = None,
    ) -> pd.DataFrame:

        try:
            self.verify_stack_id(stack_id)
        except ValueError:
            return pd.DataFrame()

        df = apply_pipeline(
            self.collection, accel_error_events_pipeline(stack_id)
        )

        if df.empty:
            logging.fatal("!!! No data collected with the pipeline !!!")
            logging.fatal("Please report this error")
            return df

        df = self.data_processor.explode_events(df)
        df = self.data_processor.normalize_field(
            df,
            target_field="additional_args",
            normalized_field="additional_args_str",
        )

        pivot = self.data_processor.create_pivot_table(df)
        result_df = self.data_processor.merge_base_with_pivot(df, pivot)

        if draw_html:
            self.visualizer.create_scatter_plot(result_df, draw_html)

        return result_df

    def get_computed_values(
        self,
        method_name: str,
        series_name: str,
        stack_id: str | None = None,
        draw_html: pathlib.Path | None = None,
    ) -> pd.DataFrame:

        try:
            self.verify_stack_id(stack_id)
            self.verify_series_name(series_name)
            self.verify_method_name(method_name)
        except ValueError:
            return pd.DataFrame()

        df = apply_pipeline(
            self.collection,
            computed_values_pipeline(series_name, method_name, stack_id),
        )

        if df.empty:
            logging.fatal("!!! No data collected with the pipeline !!!")
            logging.fatal(
                "It may mean a combinaton of this method and series is absent or pipeline failed"
            )
            logging.fatal("Please report this error")
            return df

        df = self.data_processor.normalize_field(
            df,
            target_field="accel_additional_args",
            normalized_field="accel_additional_args_str",
        )
        df = self.data_processor.normalize_field(
            df,
            target_field="series_args",
            normalized_field="series_args_str",
        )
        df = self.data_processor.provide_variation_field(df)

        if draw_html:
            self.visualizer.create_computed_line_plot(
                df, series_name, method_name, draw_html
            )

        return df

    def verify_stack_id(self, stack_id: str | None = None):
        available_stack_ids = self.get_stack_ids()
        if stack_id is not None and stack_id not in available_stack_ids:
            logging.error(
                "There are no documents in a DB with the provided stack_id: [%s]",
                stack_id,
            )
            logging.error("Available stack_id's:")
            logging.error(available_stack_ids)
            raise ValueError

    def verify_series_name(self, series_name: str):
        available_series_names = self.get_series_names()
        if series_name not in available_series_names:
            logging.error(
                "There are no documents in a DB with the provided series_name: [%s]",
                series_name,
            )
            logging.error("Available series_name's:")
            logging.error(available_series_names)
            raise ValueError

    def verify_method_name(self, method_name: str):
        available_method_names = self.get_method_names()
        if method_name not in available_method_names:
            logging.error(
                "There are no documents in a DB with the provided method_name: [%s]",
                method_name,
            )
            logging.error("Available method_name's:")
            logging.error(available_method_names)
            raise ValueError

    def get_series_names(self, stack_id: str | None = None) -> list[str]:
        return apply_pipeline(
            self.collection, unqiue_fields_pipeline("series.name", stack_id)
        )["name"].to_list()

    def get_method_names(self, stack_id: str | None = None) -> list[str]:
        return apply_pipeline(
            self.collection, unqiue_fields_pipeline("accel.name", stack_id)
        )["name"].to_list()

    def get_stack_ids(self) -> list[str]:
        return apply_pipeline(
            self.collection, unqiue_fields_pipeline("stack_id")
        )["stack_id"].to_list()
