import json
import pathlib
from typing import Any, Dict, List, Optional

import pandas as pd

from src.viz.processor import DataProcessor
from src.viz.visualizer import DataVisualizer


class SingleFileDataCollector:
    def __init__(self, file_path: pathlib.Path):
        self.file_path = file_path
        self.data_processor = DataProcessor()
        self.visualizer = DataVisualizer()
        self._data_cache: Optional[List[Dict[str, Any]]] = None

    def _load_data(self) -> List[Dict[str, Any]]:
        if self._data_cache is not None:
            return self._data_cache
        
        if not self.file_path.exists():
            raise FileNotFoundError(f"Data file not found: {self.file_path}")
        
        with open(self.file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
            
        if isinstance(data, dict):
            self._data_cache = [data]
        elif isinstance(data, list):
            self._data_cache = data
        else:
            raise ValueError(f"Invalid data format in {self.file_path}")
        
        return self._data_cache

    def _filter_data(self, data: List[Dict[str, Any]], filters: Dict[str, Any]) -> List[Dict[str, Any]]:
        return [
            item for item in data
            if all(
                item.get(key) == value or 
                (isinstance(value, dict) and item.get(key, {}).get(list(value.keys())[0]) == list(value.values())[0])
                for key, value in filters.items() if value is not None
            )
        ]

    def get_error_and_event_stats_per_method(
        self,
        stack_id: Optional[str] = None,
        draw_html: Optional[pathlib.Path] = None,
    ) -> pd.DataFrame:
        
        data = self._load_data()
        data = self._filter_data(data, {'stack_id': stack_id})
        
        if not data:
            return pd.DataFrame()
        
        # Transform data to match MongoDB pipeline output format
        mongo_like_data = []
        for item in data:
            accel = item.get('accel', {})
            series = item.get('series', {})
            
            mongo_like_data.append({
                'method_name': accel.get('name'),
                'm_value': accel.get('m_value'),
                'additional_args': accel.get('additional_args', {}),
                'additional_args_str': json.dumps(accel.get('additional_args', {}), sort_keys=True, ensure_ascii=False),
                'error': item.get('error', False),
                'slow_accel_method': item.get('slow_accel_method', 0),
                'divergent_accel_method': item.get('divergent_accel_method', 0),
                'events': item.get('events', []),
                'total': 1,
                'errors': 1 if item.get('error') else 0,
                'error_rate': 1.0 if item.get('error') else 0.0,
                'series_name': series.get('name'),
                'stack_id': item.get('stack_id')
            })
        
        df = pd.DataFrame(mongo_like_data)
        
        if df.empty:
            return df
        
        # Apply existing data processing pipeline
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
        stack_id: Optional[str] = None,
        draw_html: Optional[pathlib.Path] = None,
    ) -> pd.DataFrame:
        
        data = self._load_data()
        data = self._filter_data(data, {'stack_id': stack_id})
        
        # Filter by method and series
        data = [
            item for item in data
            if (item.get('accel', {}).get('name') == method_name and 
                item.get('series', {}).get('name') == series_name)
        ]
        
        if not data:
            return pd.DataFrame()
        
        # Extract computed data
        computed_data = []
        for item in data:
            accel = item.get('accel', {})
            series = item.get('series', {})
            computed = item.get('computed', [])
            
            for comp in computed:
                if isinstance(comp, dict):
                    computed_data.append({
                        'n': comp.get('n'),
                        'm': accel.get('m_value'),
                        'series_name': series.get('name'),
                        'stack_id': item.get('stack_id'),
                        'lim': series.get('lim'),
                        'partial_sum': comp.get('partial_sum'),
                        'accel_value': comp.get('accel_value'),
                        'partial_sum_deviation': comp.get('partial_sum_deviation'),
                        'accel_value_deviation': comp.get('accel_value_deviation'),
                        'accel_additional_args': accel.get('additional_args', {}),
                        'series_args': series.get('arguments', {})
                    })
        
        df = pd.DataFrame(computed_data)
        
        if df.empty:
            return df
        
        # Apply existing data processing pipeline
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

    def get_series_names(self, stack_id: Optional[str] = None) -> List[str]:
        data = self._load_data()
        data = self._filter_data(data, {'stack_id': stack_id})
        
        series_names = set()
        for item in data:
            series = item.get('series', {})
            if 'name' in series:
                series_names.add(series['name'])
        
        return sorted(list(series_names))

    def get_method_names(self, stack_id: Optional[str] = None) -> List[str]:
        data = self._load_data()
        data = self._filter_data(data, {'stack_id': stack_id})
        
        method_names = set()
        for item in data:
            accel = item.get('accel', {})
            if 'name' in accel:
                method_names.add(accel['name'])
        
        return sorted(list(method_names))

    def get_stack_ids(self) -> List[str]:
        data = self._load_data()
        
        stack_ids = set()
        for item in data:
            if 'stack_id' in item:
                stack_ids.add(item['stack_id'])
        
        return sorted(list(stack_ids))
