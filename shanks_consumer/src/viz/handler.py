import logging
from typing import Optional

from src.config import VizConfig
from src.db import MongoDatabase
from src.viz.collector import MongoDataCollector
from src.viz.single_file_collector import SingleFileDataCollector


def handle_viz_command(config: VizConfig, mongo_database: MongoDatabase | None = None):
    if config.from_file:
        # Use file-based data source as specified by flag
        logging.info("Using file-based data source: %s", config.from_file)
        if not config.from_file.exists():
            raise FileNotFoundError(f"Data file not found: {config.from_file}")
        collector = SingleFileDataCollector(config.from_file)
    else:
        logging.info("Using MongoDB data source")
        if mongo_database is None:
            raise RuntimeError("MongoDB data source is required when --from-file is not specified")
        collector = MongoDataCollector(mongo_database, config.mongo_collection)

    if config.with_summary:
        logging.info("Creating summary...")
        df = collector.get_error_and_event_stats_per_method(
            stack_id=config.stack_id, draw_html=config.summary_filename
        )
        if not df.empty:
            logging.info("Written summary into %s", config.summary_filename)

    if config.series_name and config.method_name:
        logging.info(
            "Drawing plots for %s series and %s method...",
            config.series_name,
            config.method_name,
        )
        df = collector.get_computed_values(
            series_name=config.series_name,
            method_name=config.method_name,
            stack_id=config.stack_id,
            draw_html=config.computed_filename,
        )
        if not df.empty:
            logging.info("Written plot into %s", config.computed_filename)
    else:
        logging.warning("Provide a series and a method to plot it")
        logging.warning(collector.get_method_names())
        logging.warning(collector.get_series_names())
