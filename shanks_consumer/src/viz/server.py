import argparse
import json
import re
import urllib.parse
from http.server import HTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
from typing import Any, Dict, List, Protocol, Set, Union

import pandas as pd
import pyarrow as pa
import pyarrow.compute as pc
import pyarrow.dataset as ds


class DataLoader(Protocol):
    """Common interface for data loaders."""

    def get_metadata(self) -> Dict[str, Any]:
        """Get metadata for UI generation."""
        ...

    def filter_data(self, filters: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Filter data based on query parameters."""
        ...


class JsonDataLoader:
    """JSON data loader implementing DataLoader interface."""

    def __init__(self, data: List[Dict[str, Any]]):
        self.data = preprocess_data(data)
        self._metadata_cache = None

    def get_metadata(self) -> Dict[str, Any]:
        """Extract metadata from JSON data."""
        if self._metadata_cache is None:
            self._metadata_cache = self._extract_metadata_from_data(self.data)
        return self._metadata_cache

    def _extract_metadata_from_data(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Extract metadata from data list."""
        series_names = set()
        accel_methods = set()
        m_values = set()
        additional_params = {}
        series_params = {}
        precisions = set()
        base_series_names = set()
        base_accel_names = set()
        series_param_info = {}
        accel_param_info = {}

        for item in data:
            # Extract series info
            if item.get("series", {}).get("name"):
                series_name = item["series"]["name"]
                series_names.add(series_name)

                precision = item.get("precision")
                if precision:
                    precisions.add(precision)

                base_series_name = series_name
                base_series_names.add(base_series_name)

                if item.get("series", {}).get("arguments"):
                    series_args = item["series"]["arguments"]
                    if series_name not in series_param_info:
                        series_param_info[series_name] = []

                    for param_name in series_args.keys():
                        if param_name not in series_param_info[series_name]:
                            series_param_info[series_name].append(param_name)

                        if param_name not in series_params:
                            series_params[param_name] = set()
                        series_params[param_name].add(str(series_args[param_name]))

            # Extract accel info
            if item.get("accel", {}).get("name"):
                accel_name = item["accel"]["name"]
                accel_methods.add(accel_name)

                precision = item.get("precision")
                if precision:
                    precisions.add(precision)
                base_accel_name = accel_name
                base_accel_names.add(base_accel_name)

                if item.get("accel", {}).get("additional_args"):
                    accel_args = item["accel"]["additional_args"]
                    if accel_name not in accel_param_info:
                        accel_param_info[accel_name] = []

                    for param_name in accel_args.keys():
                        if param_name not in accel_param_info[accel_name]:
                            accel_param_info[accel_name].append(param_name)

                        if param_name not in additional_params:
                            additional_params[param_name] = set()
                        additional_params[param_name].add(str(accel_args[param_name]))

                if item.get("accel", {}).get("m_value") is not None:
                    m_values.add(item["accel"]["m_value"])

        return {
            "series_names": sorted(list(series_names)),
            "accel_methods": [{"name": name} for name in sorted(list(accel_methods))],
            "m_values": sorted(list(m_values)),
            "additional_params": {
                k: sorted(list(v)) for k, v in additional_params.items()
            },
            "series_params": {k: sorted(list(v)) for k, v in series_params.items()},
            "precisions": sorted(list(precisions)),
            "base_series_names": sorted(list(base_series_names)),
            "base_accel_names": sorted(list(base_accel_names)),
            "series_param_info": series_param_info,
            "accel_param_info": accel_param_info,
        }

    def filter_data(self, filters: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Filter JSON data using original logic."""
        # Extract filter parameters
        series_filter = filters.get("series_filter", [])
        methods_filter = filters.get("methods_filter", [])
        m_values_filter = filters.get("m_values_filter", [])
        precision_filter = filters.get("precision_filter", [])
        base_series_filter = filters.get("base_series_filter", [])
        base_accel_filter = filters.get("base_accel_filter", [])
        accel_params = filters.get("accel_params", {})
        series_params = filters.get("series_params", {})

        # Group data by (series, accel) for selecting best stack_id
        grouped_data = {}

        for item in self.data:
            if not item.get("computed") or len(item["computed"]) == 0:
                continue

            # Create grouping key based on parameters
            series_key = item.get("series", {}).get("name", "")
            accel_key = item.get("accel", {}).get("name", "")
            m_value = item.get("accel", {}).get("m_value")

            # Extract precision and base names for filtering
            precision = item.get("precision")
            series_precision = precision
            accel_precision = precision
            base_series_name = series_key
            base_accel_name = accel_key

            # Check filters
            if series_filter and series_key not in series_filter:
                continue
            if methods_filter and accel_key not in methods_filter:
                continue
            if m_values_filter and m_value not in m_values_filter:
                continue

            # Check precision filter
            if precision_filter:
                if not accel_precision or accel_precision not in precision_filter:
                    continue
                if series_precision and series_precision not in precision_filter:
                    continue

            # Check base series filter
            if base_series_filter and base_series_name not in base_series_filter:
                continue

            # Check base accel filter
            if base_accel_filter and base_accel_name not in base_accel_filter:
                continue

            # Check accel additional args
            accel_match = True
            item_additional_args = item.get("accel", {}).get("additional_args", {})
            for param_name, expected_values in accel_params.items():
                if expected_values:
                    if param_name in item_additional_args:
                        actual_value = str(item_additional_args[param_name])
                        if actual_value not in expected_values:
                            accel_match = False
                            break

            if not accel_match:
                continue

            # Check series args
            series_match = True
            item_series_args = item.get("series", {}).get("arguments", {})
            for param_name, expected_values in series_params.items():
                if expected_values:
                    if param_name in item_series_args:
                        actual_value = str(item_series_args[param_name])
                        if actual_value not in expected_values:
                            series_match = False
                            break

            if not series_match:
                continue

            # Create grouping key
            additional_args_str = json.dumps(
                item.get("accel", {}).get("additional_args", {}), sort_keys=True
            )
            series_args_str = json.dumps(
                item.get("series", {}).get("arguments", {}), sort_keys=True
            )
            group_key = (
                series_key,
                accel_key,
                m_value,
                additional_args_str,
                series_args_str,
            )

            if group_key not in grouped_data:
                grouped_data[group_key] = []

            grouped_data[group_key].append(item)

        # Select best item from each group (minimal final error)
        filtered = []
        for group_items in grouped_data.values():
            best_item = min(group_items, key=lambda x: self._get_final_error(x))
            filtered.append(best_item)

        return filtered

    def _get_final_error(self, item: Dict[str, Any]) -> float:
        """Get final error for item."""
        if not item.get("computed") or len(item["computed"]) == 0:
            return float("inf")

        last_computed = item["computed"][-1]
        deviation_value = last_computed.get("accel_value_deviation", float("inf"))
        return abs(parse_complex_number(deviation_value))


import pyarrow as pa
import pyarrow.compute as pc
import pyarrow.dataset as ds


def parse_complex_number(value: Union[str, float, int]) -> float:
    """Parse complex numbers and scientific notation strings to float.

    For visualization purposes, we extract the real part of complex numbers.

    Examples:
    - "1.64872127070018063164E0 + 0E0 * i" -> 1.64872127070018063164
    - "5E-1 + 0E0 * i" -> 5E-1 -> 0.5
    - "1.2351 + 1246234.65 * i" -> 1.2351
    - "5.248481770719903848E-14" -> 5.248481770719903848e-14
    - "1.5135321E5 + 236321E-4 * i" -> 1.5135321E5 -> 151353.21
    - 1.5 -> 1.5
    """
    if isinstance(value, (int, float)):
        return float(value)

    if not isinstance(value, str):
        return float("inf")

    value = value.strip()

    # Handle complex numbers in format "real + imag * i"
    if " + " in value and " * i" in value:
        # Extract real part from complex number
        real_part = value.split(" + ")[0].strip()
        return parse_scientific_notation(real_part)

    # Handle complex numbers in format "real - imag * i" (negative imaginary)
    if " - " in value and " * i" in value:
        # Extract real part from complex number
        real_part = value.split(" - ")[0].strip()
        return parse_scientific_notation(real_part)

    # Handle scientific notation like "5.248481770719903848E-14"
    return parse_scientific_notation(value)


def parse_scientific_notation(value: str) -> float:
    """Parse scientific notation strings to float."""
    try:
        return float(value)
    except ValueError:
        # Handle cases like "1.64872127070018063164E0"
        if "E" in value:
            # Python can handle this format directly
            try:
                return float(value.replace("E", "e"))
            except ValueError:
                pass

        # Try to extract number using regex
        match = re.search(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?", value)
        if match:
            try:
                return float(match.group())
            except ValueError:
                pass

        return float("inf")


class DataAPIHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, data_loader=None, **kwargs):
        self.data_loader = data_loader  # Can be JsonDataLoader or ParquetDataLoader
        super().__init__(*args, **kwargs)

    def get_data(self):
        """Возвращает предзагруженные данные"""
        return self.data_loader

    def do_GET(self):
        if self.path.startswith("/api/data"):
            self.handle_api_request()
        elif self.path.startswith("/api/metadata"):
            self.handle_metadata_request()
        elif self.path == "/":
            self.serve_html_from_memory()
        else:
            self.send_error(404, "File not found")

    def serve_html_from_memory(self):
        """Обслуживает HTML контент из памяти"""
        html_content = self.generate_dynamic_html()

        self.send_response(200)
        self.send_header("Content-type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(html_content.encode("utf-8"))

    def generate_dynamic_html(self):
        """Генерирует HTML динамически на основе метаданных"""
        metadata = self.extract_metadata()

        # Создаем кнопки для precision (все включены по умолчанию)
        precision_buttons = []
        for precision in metadata["precisions"]:
            precision_buttons.append(f'''
                <button class="filter-btn precision-btn active" data-type="precision" data-value="{precision}">{precision}</button>
            ''')

        # Создаем кнопки для базовых имен рядов (все выключены по умолчанию)
        base_series_buttons = []
        for base_series in metadata["base_series_names"]:
            # Ищем любой series с этим базовым именем, чтобы получить информацию о параметрах
            # Идиотизм ли это? ДА. FIXME.
            param_info = ""
            for series_name in metadata["series_names"]:
                if series_name == base_series and series_name in metadata.get(
                    "series_param_info", {}
                ):
                    params = metadata["series_param_info"][series_name]
                    if params:
                        param_info = f" ({', '.join(params)})"
                    break
            base_series_buttons.append(f'''
                <button class="filter-btn base-series-btn" data-type="base_series" data-value="{base_series}" title="Параметры: {param_info.strip(" ()")}">{base_series}{param_info}</button>
            ''')

        # Создаем кнопки для базовых имен методов ускорения (все выключены по умолчанию)
        base_accel_buttons = []
        for base_accel in metadata["base_accel_names"]:
            # Ищем любой accel с этим базовым именем, чтобы получить информацию о параметрах
            param_info = ""
            for accel_name in [method["name"] for method in metadata["accel_methods"]]:
                if accel_name == base_accel and accel_name in metadata.get(
                    "accel_param_info", {}
                ):
                    params = metadata["accel_param_info"][accel_name]
                    if params:
                        param_info = f" ({', '.join(params)})"
                    break
            base_accel_buttons.append(f'''
                <button class="filter-btn base-accel-btn" data-type="base_accel" data-value="{base_accel}" title="Параметры: {param_info.strip(" ()")}">{base_accel}{param_info}</button>
            ''')

        # Создаем кнопки для m_values (все включены по умолчанию)
        mvalue_buttons = []
        for m_value in metadata["m_values"]:
            mvalue_buttons.append(f'''
                <button class="filter-btn mvalue-btn active" data-type="mvalue" data-value="{m_value}">{float(m_value):.6g}</button>
            ''')

        # Создаем фильтры для дополнительных параметров
        additional_filters = ""
        for param_name, param_values in metadata["additional_params"].items():
            param_buttons = []
            for value in param_values:
                # Try to format as number if possible
                try:
                    formatted_value = f"{float(value):.6g}"
                except (ValueError, TypeError):
                    formatted_value = str(value)
                param_buttons.append(f'''
                    <button class="filter-btn param-btn active" data-type="param_{param_name}" data-value="{value}">{param_name}: {formatted_value}</button>
                ''')

            additional_filters += f"""
                <div class="filter-group">
                    <div class="filter-header">
                        <label>{param_name}:</label>
                        <div class="toggle-buttons">
                            <button class="toggle-btn" onclick="toggleAll('param_{param_name}', true)">Включить все</button>
                            <button class="toggle-btn" onclick="toggleAll('param_{param_name}', false)">Выключить все</button>
                        </div>
                    </div>
                    <div class="button-group">
                        {"".join(param_buttons)}
                    </div>
                </div>
            """

        # Создаем фильтры для параметров рядов
        series_param_filters = ""
        for param_name, param_values in metadata["series_params"].items():
            param_buttons = []
            for value in param_values:
                # Try to format as number if possible
                try:
                    formatted_value = f"{float(value):.6g}"
                except (ValueError, TypeError):
                    formatted_value = str(value)
                param_buttons.append(f'''
                    <button class="filter-btn series-param-btn active" data-type="series_param_{param_name}" data-value="{value}">{param_name}: {formatted_value}</button>
                ''')

            series_param_filters += f"""
                <div class="filter-group">
                    <div class="filter-header">
                        <label>Series {param_name}:</label>
                        <div class="toggle-buttons">
                            <button class="toggle-btn" onclick="toggleAll('series_param_{param_name}', true)">Включить все</button>
                            <button class="toggle-btn" onclick="toggleAll('series_param_{param_name}', false)">Выключить все</button>
                        </div>
                    </div>
                    <div class="button-group">
                        {"".join(param_buttons)}
                    </div>
                </div>
            """

        return f"""
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Интерактивный дашборт (API режим)</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #1e1e1e;
            color: #ffffff;
        }}
        .container {{
            max-width: 1600px;
            margin: 0 auto;
        }}
        .filters {{
            background-color: #2d2d2d;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }}
        .filter-group {{
            margin-bottom: 15px;
        }}
        .filter-group label {{
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }}
        .checkbox-group {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 10px;
        }}
        .checkbox-item {{
            background-color: #3d3d3d;
            padding: 8px;
            border-radius: 4px;
            border: 1px solid #555;
        }}
        .checkbox-item input {{
            margin-right: 8px;
        }}
        .button-group {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 8px;
        }}
        .filter-btn {{
            background-color: #3d3d3d;
            color: #ffffff;
            padding: 10px 16px;
            border: 1px solid #555;
            border-radius: 4px;
            cursor: pointer;
            font-size: 14px;
            transition: all 0.2s ease;
        }}
        .filter-btn:hover {{
            background-color: #4d4d4d;
            border-color: #666;
        }}
        .filter-btn.active {{
            background-color: #007acc;
            border-color: #005a9e;
            color: white;
        }}
        .filter-btn.active:hover {{
            background-color: #005a9e;
        }}
        .filter-header {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }}
        .toggle-buttons {{
            display: flex;
            gap: 5px;
        }}
        .toggle-btn {{
            background-color: #555;
            color: #ffffff;
            padding: 5px 10px;
            border: 1px solid #666;
            border-radius: 3px;
            cursor: pointer;
            font-size: 12px;
            transition: all 0.2s ease;
        }}
        .toggle-btn:hover {{
            background-color: #666;
            border-color: #777;
        }}
        .plot-container {{
            background-color: #2d2d2d;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
        }}
        .update-btn {{
            background-color: #007acc;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
        }}
        .update-btn:hover {{
            background-color: #005a9e;
        }}
        .series-selector {{
            background-color: #2d2d2d;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
            text-align: center;
        }}
        .series-btn {{
            background-color: #007acc;
            color: white;
            padding: 8px 16px;
            margin: 5px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 14px;
        }}
        .series-btn:hover {{
            background-color: #005a9e;
        }}
        .series-btn.active {{
            background-color: #00a86b;
        }}
        .stats {{
            background-color: #2d2d2d;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Интерактивный дашборт</h1>

        <div class="filters">
            <h2>Фильтры</h2>

            <div class="filter-group">
                <div class="filter-header">
                    <label>Точность:</label>
                    <div class="toggle-buttons">
                        <button class="toggle-btn" onclick="toggleAll('precision', true)">Включить все</button>
                        <button class="toggle-btn" onclick="toggleAll('precision', false)">Выключить все</button>
                    </div>
                </div>
                <div class="button-group">
                    {"".join(precision_buttons)}
                </div>
            </div>

            <div class="filter-group">
                <div class="filter-header">
                    <label>Базовые ряды:</label>
                    <div class="toggle-buttons">
                        <button class="toggle-btn" onclick="toggleAll('base_series', true)">Включить все</button>
                        <button class="toggle-btn" onclick="toggleAll('base_series', false)">Выключить все</button>
                    </div>
                </div>
                <div class="button-group">
                    {"".join(base_series_buttons)}
                </div>
            </div>

            <div class="filter-group">
                <div class="filter-header">
                    <label>Базовые методы ускорения:</label>
                    <div class="toggle-buttons">
                        <button class="toggle-btn" onclick="toggleAll('base_accel', true)">Включить все</button>
                        <button class="toggle-btn" onclick="toggleAll('base_accel', false)">Выключить все</button>
                    </div>
                </div>
                <div class="button-group">
                    {"".join(base_accel_buttons)}
                </div>
            </div>

            <div class="filter-group">
                <div class="filter-header">
                    <label>Значения m:</label>
                    <div class="toggle-buttons">
                        <button class="toggle-btn" onclick="toggleAll('mvalue', true)">Включить все</button>
                        <button class="toggle-btn" onclick="toggleAll('mvalue', false)">Выключить все</button>
                    </div>
                </div>
                <div class="button-group">
                    {"".join(mvalue_buttons)}
                </div>
            </div>

            {additional_filters}

            {series_param_filters}

            <button class="update-btn" onclick="updatePlots()">Обновить графики</button>
        </div>

        <div class="stats" id="stats">
            <h3>Статистика</h3>
            <p>Выберите ряды и нажмите "Обновить графики"</p>
        </div>

        <div class="plot-container">
            <div id="convergence-plot"></div>
        </div>

        <div class="plot-container">
            <div id="error-plot"></div>
        </div>

        <div class="plot-container">
            <div id="performance-plot"></div>
        </div>
    </div>

    <script>
        let currentData = [];
        let metadata = {json.dumps(metadata)};

        document.addEventListener('DOMContentLoaded', function() {{
            setupFilterButtons();
        }});

        function setupFilterButtons() {{
            // Setup all filter buttons
            const filterButtons = document.querySelectorAll('.filter-btn');
            filterButtons.forEach(btn => {{
                btn.addEventListener('click', function() {{
                    this.classList.toggle('active');
                }});
            }});
        }}

        function toggleAll(type, activate) {{
            const buttons = document.querySelectorAll(`.filter-btn[data-type="${{type}}"]`);
            buttons.forEach(btn => {{
                if (activate) {{
                    btn.classList.add('active');
                }} else {{
                    btn.classList.remove('active');
                }}
            }});
        }}

        function getSelectedValues(name) {{
            // For checkboxes (m_values and additional params)
            const checkboxes = document.querySelectorAll(`input[name="${{name}}"]:checked`);
            if (checkboxes.length > 0) {{
                return Array.from(checkboxes).map(cb => cb.value);
            }}

            // For filter buttons
            const buttons = document.querySelectorAll(`.filter-btn[data-type="${{name}}"].active`);
            return Array.from(buttons).map(btn => btn.getAttribute('data-value'));
        }}

        function getAllSelectedParams() {{
            const params = new URLSearchParams();

            // Добавляем precision
            getSelectedValues('precision').forEach(precision => params.append('precision', precision));

            // Добавляем базовые ряды
            getSelectedValues('base_series').forEach(baseSeries => params.append('base_series', baseSeries));

            // Добавляем базовые методы ускорения
            getSelectedValues('base_accel').forEach(baseAccel => params.append('base_accel', baseAccel));

            // Добавляем m_values
            getSelectedValues('mvalue').forEach(m => params.append('m_value', m));

            // Добавляем дополнительные параметры
            Object.keys(metadata.additional_params).forEach(paramName => {{
                getSelectedValues(`param_${{paramName}}`).forEach(value => {{
                    params.append(`accel_param_${{paramName}}`, value);
                }});
            }});

            // Добавляем параметры рядов
            Object.keys(metadata.series_params).forEach(paramName => {{
                getSelectedValues(`series_param_${{paramName}}`).forEach(value => {{
                    params.append(`series_param_${{paramName}}`, value);
                }});
            }});

            return params;
        }}

        async function updatePlots() {{
            const params = getAllSelectedParams();

            try {{
                const response = await fetch(`/api/data?${{params.toString()}}`);
                currentData = await response.json();

                updateStats();
                createPlots();

            }} catch (error) {{
                console.error('Error loading data:', error);
                document.getElementById('stats').innerHTML = `
                    <h3>Ошибка</h3>
                    <p>Не удалось загрузить данные: ${{error.message}}</p>
                `;
            }}
        }}

        function updateStats() {{
            const statsDiv = document.getElementById('stats');
            const totalItems = currentData.length;
            const uniqueSeries = [...new Set(currentData.map(item => item.series?.name))].length;
            const uniqueMethods = [...new Set(currentData.map(item => item.accel?.name))].length;

            const selectedPrecisions = getSelectedValues('precision');
            const selectedBaseSeries = getSelectedValues('base_series');
            const selectedBaseAccel = getSelectedValues('base_accel');

            const precisionText = selectedPrecisions.length > 0 ? selectedPrecisions.join(', ') : 'не выбраны';
            const baseSeriesText = selectedBaseSeries.length > 0 ? selectedBaseSeries.join(', ') : 'не выбраны';
            const baseAccelText = selectedBaseAccel.length > 0 ? selectedBaseAccel.join(', ') : 'не выбраны';

            statsDiv.innerHTML = `
                <h3>Статистика</h3>
                <p>Точности: ${{precisionText}}</p>
                <p>Базовые ряды: ${{baseSeriesText}}</p>
                <p>Базовые методы: ${{baseAccelText}}</p>
                <p>Всего записей: ${{totalItems}}</p>
                <p>Уникальных рядов: ${{uniqueSeries}}</p>
                <p>Уникальных методов: ${{uniqueMethods}}</p>
            `;
        }}

        function roundNumber(num, precision = 6) {{
            if (typeof num === 'number') {{
                return parseFloat(num.toFixed(precision));
            }}
            if (typeof num === 'string') {{
                // Try to parse string as number
                const parsed = parseFloat(num);
                if (!isNaN(parsed)) {{
                    return parseFloat(parsed.toFixed(precision));
                }}
            }}
            return num; // Return as-is if not a number
        }}

        function formatItemName(item) {{
            // Format accel name with parameters
            let accelName = item.accel.name;
            if (item.accel.additional_args && Object.keys(item.accel.additional_args).length > 0) {{
                const accelParams = Object.entries(item.accel.additional_args)
                    .map(([key, value]) => key + "=" + roundNumber(value))
                    .join(', ');
                accelName += " (" + accelParams + ")";
            }}

            // Format series name with parameters
            let seriesName = item.series.name;
            if (item.series.arguments && Object.keys(item.series.arguments).length > 0) {{
                const seriesParams = Object.entries(item.series.arguments)
                    .map(([key, value]) => key + "=" + roundNumber(value))
                    .join(', ');
                seriesName += " (" + seriesParams + ")";
            }}

            return accelName + " (m=" + roundNumber(item.accel.m_value) + ") " + seriesName;
        }}

        function createPlots() {{
            if (currentData.length === 0) return;

            createConvergencePlot();
            createErrorPlot();
            createPerformancePlot();
        }}

        function createConvergencePlot() {{
            const traces = [];
            const partialSumTraces = new Map(); // Store one partial sum trace per series
            const limitTraces = new Map(); // Store one limit trace per series

            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;

                const computed = item.computed;
                const variation = formatItemName(item);

                // Handle complex numbers - check if values are objects with real/imag parts
                const hasComplex = computed.some(c =>
                    typeof c.accel_value === 'object' && c.accel_value !== null);

                if (hasComplex) {{
                    // Complex number visualization - show real and imaginary parts
                    traces.push({{
                        x: computed.map(c => c.n),
                        y: computed.map(c => c.accel_value.real),
                        mode: 'lines+markers',
                        name: variation,
                        line: {{ width: 2 }}
                    }});

                    // Check if there are non-zero imaginary parts
                    const hasImaginary = computed.some(c =>
                        Math.abs(c.accel_value.imag || 0) > 1e-15);

                    if (hasImaginary) {{
                        traces.push({{
                            x: computed.map(c => c.n),
                            y: computed.map(c => c.accel_value.imag || 0),
                            mode: 'lines+markers',
                            name: variation + ' (мнимая часть)',
                            line: {{ width: 2, dash: 'dot' }}
                        }});
                    }}

                    // Store partial sums for this series (only once per series)
                    if (!partialSumTraces.has(item.series.name)) {{
                        partialSumTraces.set(item.series.name, {{
                            x: computed.map(c => c.n),
                            y: computed.map(c => c.partial_sum.real),
                            name: item.series.name + ' (частичные суммы, действительная часть)',
                            line: {{ dash: 'dash', width: 1 }},
                            marker: {{ size: 4, symbol: 'x' }}
                        }});

                        if (hasImaginary) {{
                            partialSumTraces.get(item.series.name).imagTrace = {{
                                x: computed.map(c => c.n),
                                y: computed.map(c => (c.partial_sum.imag || 0)),
                                name: item.series.name + ' (частичные суммы, мнимая часть)',
                                line: {{ dash: 'dash', width: 1, dot: 'dot' }},
                                marker: {{ size: 4, symbol: 'triangle-up' }}
                            }};
                        }}
                    }}

                    // Store limit line for this series (only once per series)
                    if (!limitTraces.has(item.series.name) && item.series.lim !== undefined) {{
                        const limitValue = typeof item.series.lim === 'object' ? item.series.lim.real : item.series.lim;
                        const xRange = computed.map(c => c.n);
                        const minX = Math.min(...xRange);
                        const maxX = Math.max(...xRange);

                        limitTraces.set(item.series.name, {{
                            x: [minX, maxX],
                            y: [limitValue, limitValue],
                            name: item.series.name + ' (предел)',
                            line: {{ color: '#ff6b6b', width: 3, dash: 'solid' }},
                            mode: 'lines'
                        }});
                    }}
                }} else {{
                    // Real number visualization (original logic)
                    traces.push({{
                        x: computed.map(c => c.n),
                        y: computed.map(c => c.accel_value),
                        mode: 'lines+markers',
                        name: variation,
                        line: {{ width: 2 }}
                    }});

                    // Store partial sums for this series (only once per series)
                    if (!partialSumTraces.has(item.series.name)) {{
                        partialSumTraces.set(item.series.name, {{
                            x: computed.map(c => c.n),
                            y: computed.map(c => c.partial_sum),
                            name: item.series.name + ' (частичные суммы)',
                            line: {{ dash: 'dash', width: 1 }},
                            marker: {{ size: 4, symbol: 'x' }}
                        }});
                    }}

                    // Store limit line for this series (only once per series)
                    if (!limitTraces.has(item.series.name) && item.series.lim !== undefined) {{
                        const limitValue = typeof item.series.lim === 'object' ? item.series.lim.real : item.series.lim;
                        const xRange = computed.map(c => c.n);
                        const minX = Math.min(...xRange);
                        const maxX = Math.max(...xRange);

                        limitTraces.set(item.series.name, {{
                            x: [minX, maxX],
                            y: [limitValue, limitValue],
                            name: item.series.name + ' (предел)',
                            line: {{ color: '#ff6b6b', width: 3, dash: 'solid' }},
                            mode: 'lines'
                        }});
                    }}
                }}
            }});

            // Add partial sum traces (one per series)
            partialSumTraces.forEach(trace => {{
                traces.push(trace);
                if (trace.imagTrace) {{
                    traces.push(trace.imagTrace);
                }}
            }});

            // Add limit traces (one per series)
            limitTraces.forEach(trace => {{
                traces.push(trace);
            }});

            const layout = {{
                title: 'Сходимость методов',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Значение' }},
                template: 'plotly_dark',
                height: 900,
                showlegend: true,
                legend: {{
                    orientation: 'h',
                    y: -0.2
                }}
            }};

            Plotly.newPlot('convergence-plot', traces, layout);
        }}

        function createErrorPlot() {{
            const traces = [];

            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;

                const computed = item.computed;
                const variation = formatItemName(item);

                // Handle complex numbers - check if deviation values are objects with real/imag parts
                const hasComplex = computed.some(c =>
                    typeof c.accel_value_deviation === 'object' && c.accel_value_deviation !== null);

                if (hasComplex) {{
                    // Complex number error - use magnitude sqrt(real² + imag²)
                    const errors = computed.map(c => {{
                        const dev = c.accel_value_deviation;
                        if (typeof dev === 'object' && dev !== null) {{
                            const real = dev.real || 0;
                            const imag = dev.imag || 0;
                            return Math.sqrt(real * real + imag * imag);
                        }}
                        return Math.abs(dev || 0);
                    }});

                    traces.push({{
                        x: computed.map(c => c.n),
                        y: errors,
                        mode: 'lines+markers',
                        name: variation + ' (модуль ошибки)',
                        line: {{ width: 2 }}
                    }});
                }} else {{
                    // Real number error (original logic)
                    const errors = computed.map(c => Math.abs(c.accel_value_deviation || 0));

                    traces.push({{
                        x: computed.map(c => c.n),
                        y: errors,
                        mode: 'lines+markers',
                        name: variation,
                        line: {{ width: 2 }}
                    }});
                }}
            }});

            const layout = {{
                title: 'Ошибка сходимости',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Абсолютная ошибка' }},
                template: 'plotly_dark',
                height: 700,
                legend: {{
                    orientation: 'h',
                    y: -0.2
                }}
            }};

            Plotly.newPlot('error-plot', traces, layout);
        }}

        function createPerformancePlot() {{
            const performanceData = {{}};

            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;

                const key = formatItemName(item);

                if (!performanceData[key]) {{
                    performanceData[key] = [];
                }}

                // Handle complex numbers in error calculation
                const errors = item.computed.map(c => {{
                    const dev = c.accel_value_deviation;
                    if (typeof dev === 'object' && dev !== null) {{
                        const real = dev.real || 0;
                        const imag = dev.imag || 0;
                        return Math.sqrt(real * real + imag * imag);
                    }}
                    return Math.abs(dev || 0);
                }});

                const minError = Math.min(...errors);
                const iterationAtMinError = item.computed.find((c, index) => errors[index] === minError)?.n || 0;

                performanceData[key].push({{
                    stackId: item.stack_id,
                    minError: minError,
                    iterationAtMinError: iterationAtMinError,
                    finalError: errors[errors.length - 1]
                }});
            }});

            const traces = [];

            Object.keys(performanceData).forEach(key => {{
                const data = performanceData[key];

                traces.push({{
                    x: data.map(d => d.iterationAtMinError),
                    y: data.map(d => d.minError),
                    mode: 'markers',
                    name: key,
                    marker: {{ size: 10 }}
                }});
            }});

            const layout = {{
                title: 'Производительность методов',
                xaxis: {{ title: 'Итерация достижения минимальной ошибки' }},
                yaxis: {{ title: 'Минимальная ошибка', type: 'log' }},
                template: 'plotly_dark',
                height: 700,
                legend: {{
                    orientation: 'h',
                    y: -0.2
                }}
            }};

            Plotly.newPlot('performance-plot', traces, layout);
        }}
    </script>
</body>
</html>
        """

    def extract_metadata(self):
        """Извлекает метаданные из данных для построения динамического интерфейса"""
        if self.data_loader is None:
            return {}
        return self.data_loader.get_metadata()

    def handle_metadata_request(self):
        """Возвращает метаданные о доступных параметрах"""
        if self.data_loader is None:
            self.send_response(500)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            response = json.dumps({"error": "Data loader not available"})
            self.wfile.write(response.encode("utf-8"))
            return

        metadata = self.extract_metadata()
        self.send_json_response(metadata)

    def handle_api_request(self):
        """Обрабатывает API запросы с улучшенной фильтрацией"""
        parsed_path = urllib.parse.urlparse(self.path)
        query_params = urllib.parse.parse_qs(parsed_path.query)

        # Извлекаем параметры фильтрации
        series_filter = query_params.get("series", [])
        methods_filter = query_params.get("method", [])
        m_values_filter = query_params.get("m_value", [])
        precision_filter = query_params.get("precision", [])
        base_series_filter = query_params.get("base_series", [])
        base_accel_filter = query_params.get("base_accel", [])

        # Извлекаем дополнительные параметры ускорения
        accel_params = {}
        for key, values in query_params.items():
            if key.startswith("accel_param_"):
                param_name = key[len("accel_param_") :]
                # Handle multiple values for the same parameter
                all_values = []
                for value_list in values:
                    if value_list:
                        all_values.extend(value_list.split(","))
                accel_params[param_name] = [v.strip() for v in all_values if v.strip()]

        # Извлекаем параметры рядов
        series_params = {}
        for key, values in query_params.items():
            if key.startswith("series_param_"):
                param_name = key[len("series_param_") :]
                # Handle multiple values for the same parameter
                all_values = []
                for value_list in values:
                    if value_list:
                        all_values.extend(value_list.split(","))
                series_params[param_name] = [v.strip() for v in all_values if v.strip()]

        # Преобразуем фильтры
        if series_filter and len(series_filter) > 0:
            all_series = []
            for value_list in series_filter:
                if value_list:
                    all_series.extend(value_list.split(","))
            series_filter = [s.strip() for s in all_series if s.strip()]
        if methods_filter and len(methods_filter) > 0:
            all_methods = []
            for value_list in methods_filter:
                if value_list:
                    all_methods.extend(value_list.split(","))
            methods_filter = [m.strip() for m in all_methods if m.strip()]
        if m_values_filter and len(m_values_filter) > 0:
            all_m_values = []
            for value_list in m_values_filter:
                if value_list:
                    all_m_values.extend(value_list.split(","))
            m_values_filter = [int(x.strip()) for x in all_m_values if x.strip()]
        if precision_filter and len(precision_filter) > 0:
            all_precisions = []
            for value_list in precision_filter:
                if value_list:
                    all_precisions.extend(value_list.split(","))
            precision_filter = [p.strip() for p in all_precisions if p.strip()]
        if base_series_filter and len(base_series_filter) > 0:
            all_base_series = []
            for value_list in base_series_filter:
                if value_list:
                    all_base_series.extend(value_list.split(","))
            base_series_filter = [s.strip() for s in all_base_series if s.strip()]
        if base_accel_filter and len(base_accel_filter) > 0:
            all_base_accel = []
            for value_list in base_accel_filter:
                if value_list:
                    all_base_accel.extend(value_list.split(","))
            base_accel_filter = [a.strip() for a in all_base_accel if a.strip()]

        # Use polymorphic data loader interface
        if self.data_loader is None:
            self.send_response(500)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            response = json.dumps({"error": "Data loader not available"})
            self.wfile.write(response.encode("utf-8"))
            return

        filters = {
            "series_filter": series_filter,
            "methods_filter": methods_filter,
            "m_values_filter": m_values_filter,
            "precision_filter": precision_filter,
            "base_series_filter": base_series_filter,
            "base_accel_filter": base_accel_filter,
            "accel_params": accel_params,
            "series_params": series_params,
        }
        filtered = self.data_loader.filter_data(filters)
        self.send_json_response(filtered)

    def get_final_error(self, item):
        """Получает финальную ошибку для элемента"""
        if not item.get("computed") or len(item["computed"]) == 0:
            return float("inf")

        last_computed = item["computed"][-1]
        deviation_value = last_computed.get("accel_value_deviation", float("inf"))
        return abs(parse_complex_number(deviation_value))

    def send_json_response(self, data):
        """Отправляет JSON ответ"""
        response = json.dumps(data)
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.send_header("Content-length", str(len(response)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(response.encode("utf-8"))


def parse_complex_for_visualization(value: Union[str, float, int]) -> Dict[str, float]:
    """Parse complex numbers for visualization.

    Returns:
    - For real numbers: {'real': value, 'imag': 0.0}
    - For complex numbers: {'real': real_part, 'imag': imag_part}
    """
    if isinstance(value, (int, float)):
        return {"real": float(value), "imag": 0.0}

    if not isinstance(value, str):
        return {"real": float("inf"), "imag": 0.0}

    value = value.strip()

    # Handle complex numbers in format "real + imag * i" or "real - imag * i"
    if (" + " in value and " * i" in value) or (" - " in value and " * i" in value):
        # Extract real and imaginary parts
        if " + " in value:
            parts = value.split(" + ")
        else:
            parts = value.split(" - ")

        real_part = parse_scientific_notation(parts[0].strip())
        imag_part_str = parts[1].replace(" * i", "").strip()
        imag_part = parse_scientific_notation(imag_part_str)

        return {"real": real_part, "imag": imag_part}

    # Handle real numbers (scientific notation or regular)
    real_val = parse_scientific_notation(value)
    return {"real": real_val, "imag": 0.0}


class ParquetDataLoader:
    """Efficient parquet data loader using PyArrow with predicate pushdown."""

    def __init__(self, data_file: Path):
        self.data_file = data_file
        self.dataset = ds.dataset(data_file, format="parquet")
        self._metadata_cache = None

    def get_metadata(self) -> Dict[str, Any]:
        """Get metadata for UI generation."""
        if self._metadata_cache is None:
            # Scan entire dataset for metadata (small overhead)
            table = self.dataset.to_table(columns=["series", "accel", "stack_id"])
            self._metadata_cache = self._extract_metadata_from_table(table)
        return self._metadata_cache

    def _extract_metadata_from_table(self, table) -> Dict[str, Any]:
        """Extract metadata from PyArrow table similar to JSON logic."""
        series_names = set()
        accel_methods = set()
        m_values = set()
        additional_params = {}
        series_params = {}
        precisions = set()
        base_series_names = set()
        base_accel_names = set()
        series_param_info = {}
        accel_param_info = {}

        # Convert to pandas for easier processing (metadata is small)
        df = table.to_pandas()

        for _, row in df.iterrows():
            series = row.get("series", {})
            accel = row.get("accel", {})

            # Extract series info
            if isinstance(series, dict) and series.get("name"):
                series_name = series["name"]
                series_names.add(series_name)

                precision = row.get("precision")
                if precision:
                    precisions.add(precision)
                base_series_name = series_name
                base_series_names.add(base_series_name)

                if isinstance(series.get("arguments"), dict):
                    series_args = series["arguments"]
                    if series_name not in series_param_info:
                        series_param_info[series_name] = []

                    for param_name in series_args.keys():
                        if param_name not in series_param_info[series_name]:
                            series_param_info[series_name].append(param_name)

                        if param_name not in series_params:
                            series_params[param_name] = set()
                        series_params[param_name].add(str(series_args[param_name]))

            # Extract accel info
            if isinstance(accel, dict) and accel.get("name"):
                accel_name = accel["name"]
                accel_methods.add(accel_name)

                precision = row.get("precision")
                if precision:
                    precisions.add(precision)
                base_accel_name = accel_name
                base_accel_names.add(base_accel_name)

                if isinstance(accel.get("additional_args"), dict):
                    accel_args = accel["additional_args"]
                    if accel_name not in accel_param_info:
                        accel_param_info[accel_name] = []

                    for param_name in accel_args.keys():
                        if param_name not in accel_param_info[accel_name]:
                            accel_param_info[accel_name].append(param_name)

                        if param_name not in additional_params:
                            additional_params[param_name] = set()
                        additional_params[param_name].add(str(accel_args[param_name]))

                if accel.get("m_value") is not None:
                    m_values.add(accel["m_value"])

        return {
            "series_names": sorted(list(series_names)),
            "accel_methods": [{"name": name} for name in sorted(list(accel_methods))],
            "m_values": sorted(list(m_values)),
            "additional_params": {
                k: sorted(list(v)) for k, v in additional_params.items()
            },
            "series_params": {k: sorted(list(v)) for k, v in series_params.items()},
            "precisions": sorted(list(precisions)),
            "base_series_names": sorted(list(base_series_names)),
            "base_accel_names": sorted(list(base_accel_names)),
            "series_param_info": series_param_info,
            "accel_param_info": accel_param_info,
        }

    def filter_data(self, filters):
        """Filter data using PyArrow for simple filters, pandas for complex ones."""
        # Build simple filter expressions for PyArrow
        expressions = []

        if filters.get("series_filter"):
            expressions.append(
                ds.field("series", "name").isin(filters["series_filter"])
            )

        if filters.get("methods_filter"):
            expressions.append(
                ds.field("accel", "name").isin(filters["methods_filter"])
            )

        if filters.get("m_values_filter"):
            expressions.append(
                ds.field("accel", "m_value").isin(filters["m_values_filter"])
            )

        # Apply simple filters with PyArrow
        if expressions:
            # Use the first expression as base filter (PyArrow limitation)
            filter_expr = expressions[0]
            table = self.dataset.to_table(filter=filter_expr)
        else:
            table = self.dataset.to_table()

        # Convert to pandas for further processing
        df = table.to_pandas()

        # Apply all complex filtering in pandas
        df = self._apply_all_filters(df, filters)

        # Group and select best items (same logic as JSON)
        return self._select_best_items(df)

    def _apply_all_filters(self, df, filters):
        """Apply all filtering logic in pandas."""
        mask = pd.Series([True] * len(df))

        # Series filter
        if filters.get("series_filter"):
            series_mask = df["series"].apply(
                lambda x: x.get("name") in filters["series_filter"]
                if isinstance(x, dict)
                else False
            )
            mask = mask & series_mask

        # Methods filter
        if filters.get("methods_filter"):
            methods_mask = df["accel"].apply(
                lambda x: x.get("name") in filters["methods_filter"]
                if isinstance(x, dict)
                else False
            )
            mask = mask & methods_mask

        # M values filter
        if filters.get("m_values_filter"):
            m_values_mask = df["accel"].apply(
                lambda x: x.get("m_value") in filters["m_values_filter"]
                if isinstance(x, dict)
                else False
            )
            mask = mask & m_values_mask

        # Precision filter
        if filters.get("precision_filter"):
            precision_mask = df["precision"].isin(filters["precision_filter"])
            mask = mask & precision_mask

        # Base series filter
        if filters.get("base_series_filter"):
            base_series_mask = pd.Series([False] * len(df))
            for idx, row in df.iterrows():
                series_name = (
                    row["series"].get("name", "")
                    if isinstance(row["series"], dict)
                    else ""
                )

                for base_name in filters["base_series_filter"]:
                    if series_name.startswith(base_name):
                        base_series_mask[idx] = True
                        break
            mask = mask & base_series_mask

        # Base accel filter
        if filters.get("base_accel_filter"):
            base_accel_mask = pd.Series([False] * len(df))
            for idx, row in df.iterrows():
                accel_name = (
                    row["accel"].get("name", "")
                    if isinstance(row["accel"], dict)
                    else ""
                )

                for base_name in filters["base_accel_filter"]:
                    if accel_name.startswith(base_name):
                        base_accel_mask[idx] = True
                        break
            mask = mask & base_accel_mask

        # Filter accel additional args
        if filters.get("accel_params"):
            for param_name, expected_values in filters["accel_params"].items():
                if expected_values:
                    param_mask = df["accel"].apply(
                        lambda x: param_name in x.get("additional_args", {})
                        and str(x["additional_args"][param_name]) in expected_values
                        if isinstance(x, dict)
                        else False
                    )
                    mask = mask & param_mask

        # Filter series args
        if filters.get("series_params"):
            for param_name, expected_values in filters["series_params"].items():
                if expected_values:
                    param_mask = df["series"].apply(
                        lambda x: param_name in x.get("arguments", {})
                        and str(x["arguments"][param_name]) in expected_values
                        if isinstance(x, dict)
                        else False
                    )
                    mask = mask & param_mask

        return df[mask]

    def _select_best_items(self, df):
        """Select best items per group (same logic as JSON)."""
        if df.empty:
            return []

        # Preprocess complex numbers in computed data
        df = self._preprocess_complex_numbers(df)

        # Group by (series, accel, m_value, additional_args, series_args)
        df["group_key"] = df.apply(
            lambda row: (
                row["series"].get("name", "")
                if isinstance(row["series"], dict)
                else "",
                row["accel"].get("name", "") if isinstance(row["accel"], dict) else "",
                row["accel"].get("m_value") if isinstance(row["accel"], dict) else None,
                json.dumps(row["accel"].get("additional_args", {}), sort_keys=True)
                if isinstance(row["accel"], dict)
                else "{}",
                json.dumps(row["series"].get("arguments", {}), sort_keys=True)
                if isinstance(row["series"], dict)
                else "{}",
            ),
            axis=1,
        )

        # For each group, find item with minimal final error
        best_items = []
        for _, group in df.groupby("group_key"):
            if not group.empty:
                # Calculate final error for each item
                def get_final_error(item):
                    if (
                        not isinstance(item.get("computed"), list)
                        or len(item["computed"]) == 0
                    ):
                        return float("inf")
                    last_computed = item["computed"][-1]
                    if not isinstance(last_computed, dict):
                        return float("inf")
                    deviation = last_computed.get("accel_value_deviation", float("inf"))
                    return abs(parse_complex_number(deviation))

                best_item = group.loc[group.apply(get_final_error, axis=1).idxmin()]
                best_item_dict = best_item.to_dict()
                # Convert numpy arrays back to lists for JSON serialization
                import numpy as np

                if "computed" in best_item_dict:
                    if isinstance(best_item_dict["computed"], np.ndarray):
                        best_item_dict["computed"] = best_item_dict["computed"].tolist()
                best_items.append(best_item_dict)

        return best_items

    def _preprocess_complex_numbers(self, df):
        """Preprocess complex numbers in dataframe for visualization."""
        for idx, row in df.iterrows():
            if isinstance(row.get("computed"), list):
                for computed in row["computed"]:
                    if isinstance(computed, dict):
                        for key, value in computed.items():
                            if key in [
                                "accel_value",
                                "partial_sum",
                                "accel_value_deviation",
                                "partial_sum_deviation",
                                "series_value",
                            ]:
                                computed[key] = parse_complex_for_visualization(value)

            # Process series limit if it's a string
            if isinstance(row.get("series"), dict):
                series = row["series"]
                if "lim" in series:
                    series["lim"] = parse_complex_for_visualization(series["lim"])

        return df


def preprocess_data(data: List[Dict]) -> List[Dict]:
    """Preprocess data to convert string numbers to proper numbers for visualization."""
    for item in data:
        # Process computed values
        if "computed" in item:
            for computed in item["computed"]:
                for key, value in computed.items():
                    if key in [
                        "accel_value",
                        "partial_sum",
                        "accel_value_deviation",
                        "partial_sum_deviation",
                        "series_value",
                    ]:
                        computed[key] = parse_complex_for_visualization(value)

        # Process series limit if it's a string
        if "series" in item and "lim" in item["series"]:
            item["series"]["lim"] = parse_complex_for_visualization(
                item["series"]["lim"]
            )

        # NOTE: Don't process series arguments - keep original values for filtering
        # Series arguments are used for filtering and should remain in original format

    return data


def start_server(data_file: Path, port: int = 8000):
    """Запускает HTTP сервер с предзагруженными данными"""

    # Detect file format and create appropriate data loader
    if data_file.suffix.lower() == ".parquet":
        print(f"Loading Parquet data from {data_file}")
        data_loader = ParquetDataLoader(data_file)
        print(f"Parquet dataset loaded successfully")
    else:
        print(f"Loading JSON data from {data_file}")
        with open(data_file, "r", encoding="utf-8") as f:
            data = json.load(f)
        data_loader = JsonDataLoader(data)
        print(f"Loaded {len(data)} records from {data_file}")

    # Создаем обработчик с data loader
    def handler(*args, **kwargs):
        return DataAPIHandler(*args, data_loader=data_loader, **kwargs)

    # Запускаем сервер
    server_address = ("", port)
    httpd = HTTPServer(server_address, handler)

    print(f"\n🚀 Server started!")
    print(f"📊 Dashboard: http://localhost:{port}")
    print(f"📄 Data file: {data_file}")
    print(f"\nPress Ctrl+C to stop the server\n")

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n👋 Server stopped")
        httpd.server_close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Start dashboard server")
    parser.add_argument("--data-file", required=True, type=Path, help="JSON data file")
    parser.add_argument(
        "--port", type=int, default=8000, help="Server port (default: 8000)"
    )

    args = parser.parse_args()

    if not args.data_file.exists():
        print(f"Error: Data file {args.data_file} not found")
        exit(1)

    start_server(args.data_file, args.port)
