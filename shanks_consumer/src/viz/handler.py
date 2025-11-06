import logging
from typing import Optional

from src.config import VizConfig
from src.db import MongoDatabase
from src.viz.collector import MongoDataCollector
from src.viz.single_file_collector import SingleFileDataCollector
from src.viz.dashboard_visualizer import DashboardVisualizer


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

    # Стандартные графики только если не создаем дашборд и не сервер
    if not config.interactive_dashboard and not config.server:
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

    if config.interactive_dashboard:
        if config.all_series:
            logging.info("Creating interactive dashboard for ALL series...")
        elif config.series_name:
            logging.info("Creating interactive dashboard for %s series...", config.series_name)
        else:
            logging.warning("Interactive dashboard requires either --series-name or --all-series flag")
            return
            
        dashboard_visualizer = DashboardVisualizer()
        
        if config.from_file:
            dashboard_visualizer.load_data(config.from_file)
        elif mongo_database is not None:
            # Загружаем данные из MongoDB
            all_data = list(mongo_database.get_collection(config.mongo_collection).find({}))
            dashboard_visualizer.load_data(all_data)
        else:
            logging.error("No data source available. Use --from-file or configure MongoDB.")
            return
        
        series_name = None if config.all_series else config.series_name
        dashboard_visualizer.create_interactive_dashboard(
            series_name, 
            config.dashboard_filename
        )
        logging.info("Interactive dashboard created: %s", config.dashboard_filename)
    
    if config.server:
        if not config.from_file:
            logging.error("Server mode requires --from-file with JSON data file")
            return
            
        logging.info("Starting HTTP server with API...")
        import importlib.util
        import os
        # Get the correct path to server.py
        current_dir = os.path.dirname(os.path.abspath(__file__))
        server_path = os.path.join(current_dir, "server.py")
        spec = importlib.util.spec_from_file_location("server", server_path)
        if spec is None:
            logging.error("Could not load server module")
            return
        server_module = importlib.util.module_from_spec(spec)
        if spec.loader is None:
            logging.error("Could not load server module loader")
            return
        spec.loader.exec_module(server_module)
        server_module.start_server(config.from_file, config.port)
