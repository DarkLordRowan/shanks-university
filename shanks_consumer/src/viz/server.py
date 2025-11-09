import json
import urllib.parse
import re
from http.server import HTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
from typing import Dict, List, Set, Any, Union
import argparse


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
    def __init__(self, *args, data=None, **kwargs):
        self.data = data  # Pre-loaded data passed at server startup
        super().__init__(*args, **kwargs)

    def get_data(self):
        """Возвращает предзагруженные данные"""
        return self.data

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
                if self.extract_base_name(
                    series_name
                ) == base_series and series_name in metadata.get(
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
                if self.extract_base_name(
                    accel_name
                ) == base_accel and accel_name in metadata.get("accel_param_info", {}):
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
            // Format series name with parameters
            let seriesName = item.series.name;
            if (item.series.arguments && Object.keys(item.series.arguments).length > 0) {{
                const seriesParams = Object.entries(item.series.arguments)
                    .map(([key, value]) => key + "=" + roundNumber(value))
                    .join(', ');
                seriesName += " (" + seriesParams + ")";
            }}

            // Format accel name with parameters
            let accelName = item.accel.name;
            if (item.accel.additional_args && Object.keys(item.accel.additional_args).length > 0) {{
                const accelParams = Object.entries(item.accel.additional_args)
                    .map(([key, value]) => key + "=" + roundNumber(value))
                    .join(', ');
                accelName += " (" + accelParams + ")";
            }}

            return seriesName + " (m=" + roundNumber(item.accel.m_value) + ") " + accelName;
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
                        name: variation + ' (действительная часть)',
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
                }}
            }});

            // Add partial sum traces (one per series)
            partialSumTraces.forEach(trace => {{
                traces.push(trace);
                if (trace.imagTrace) {{
                    traces.push(trace.imagTrace);
                }}
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
                title: 'Ошибка сходимости (логарифмическая шкала)',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Абсолютная ошибка', type: 'log' }},
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

    def extract_precision(self, name: str) -> str:
        """Extract precision suffix from name (e.g., 'ExpSeriesF64' -> 'F64')"""
        # Known precision suffixes - sort by length (longer first) to avoid partial matches
        precision_suffixes = [
            "CFLong",
            "CF64",
            "CF32",
            "CArb",
            "FLong",
            "F64",
            "F32",
            "Arb",
        ]

        for suffix in precision_suffixes:
            if name.endswith(suffix):
                return suffix
        return ""  # No precision suffix found

    def extract_base_name(self, name: str) -> str:
        """Extract base name without precision suffix (e.g., 'ExpSeriesF64' -> 'ExpSeries')"""
        precision = self.extract_precision(name)
        if precision:
            return name[: -len(precision)]
        return name

    def extract_metadata(self):
        """Извлекает метаданные из данных для построения динамического интерфейса"""
        data = self.get_data()

        series_names = set()
        accel_methods = set()
        m_values = set()
        additional_params = {}
        series_params = {}
        precisions = set()
        base_series_names = set()
        base_accel_names = set()

        # Новые структуры для хранения информации о параметрах для каждой комбинации
        series_param_info = {}  # series_name -> list of param names
        accel_param_info = {}  # accel_name -> list of param names

        for item in data or []:
            # Извлекаем имя ряда
            if item.get("series", {}).get("name"):
                series_name = item["series"]["name"]
                series_names.add(series_name)

                # Extract precision and base name
                precision = self.extract_precision(series_name)
                if precision:  # Only add if precision found
                    precisions.add(precision)
                base_series_name = self.extract_base_name(series_name)
                base_series_names.add(base_series_name)

                # Извлекаем параметры ряда и сохраняем информацию о них для данного series
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

            # Извлекаем методы ускорения и их параметры
            if item.get("accel", {}).get("name"):
                accel_name = item["accel"]["name"]
                accel_methods.add(accel_name)

                # Extract precision and base name for acceleration methods
                precision = self.extract_precision(accel_name)
                if precision:  # Only add if precision found
                    precisions.add(precision)
                base_accel_name = self.extract_base_name(accel_name)
                base_accel_names.add(base_accel_name)

                # Извлекаем дополнительные параметры ускорения и сохраняем информацию о них
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

        # Конвертируем sets в sorted lists
        accel_method_list = [{"name": name} for name in sorted(list(accel_methods))]

        return {
            "series_names": sorted(list(series_names)),
            "accel_methods": accel_method_list,
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

    def handle_metadata_request(self):
        """Возвращает метаданные о доступных параметрах"""
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

        data = self.get_data()
        if data is None:
            self.send_response(500)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            response = json.dumps({"error": "Data not available"})
            self.wfile.write(response.encode("utf-8"))
            return

        # Группируем данные по (series, accel) для выбора лучшего stack_id
        grouped_data = {}

        for item in data:
            if not item.get("computed") or len(item["computed"]) == 0:
                continue

            # Создаем ключ группировки на основе параметров
            series_key = item.get("series", {}).get("name", "")
            accel_key = item.get("accel", {}).get("name", "")
            m_value = item.get("accel", {}).get("m_value")

            # Extract precision and base names for filtering
            series_precision = self.extract_precision(series_key)
            accel_precision = self.extract_precision(accel_key)
            base_series_name = self.extract_base_name(series_key)
            base_accel_name = self.extract_base_name(accel_key)

            # Проверяем фильтры
            if series_filter and series_key not in series_filter:
                continue
            if methods_filter and accel_key not in methods_filter:
                continue
            if m_values_filter and m_value not in m_values_filter:
                continue

            # Check precision filter - different logic for series and accel
            if precision_filter:
                # For acceleration methods: must have precision and match selected precisions
                if not accel_precision or accel_precision not in precision_filter:
                    continue

                # For series: if no precision, matches all selected precisions
                # If has precision, must match selected precisions
                if series_precision and series_precision not in precision_filter:
                    continue

            # Check base series filter
            if base_series_filter and base_series_name not in base_series_filter:
                continue

            # Check base accel filter
            if base_accel_filter and base_accel_name not in base_accel_filter:
                continue

            # Проверяем дополнительные параметры ускорения
            accel_match = True
            item_additional_args = item.get("accel", {}).get("additional_args", {})
            for param_name, expected_values in accel_params.items():
                if (
                    expected_values
                ):  # Only check if user specified values for this parameter
                    if param_name in item_additional_args:
                        # Item has this parameter, check if value matches
                        actual_value = str(item_additional_args[param_name])
                        if actual_value not in expected_values:
                            accel_match = False
                            break
                    # If item doesn't have this parameter, it passes the filter (user can filter by parameters they care about)

            if not accel_match:
                continue

            # Проверяем параметры рядов
            series_match = True
            item_series_args = item.get("series", {}).get("arguments", {})
            for param_name, expected_values in series_params.items():
                if (
                    expected_values
                ):  # Only check if user specified values for this parameter
                    if param_name in item_series_args:
                        # Item has this parameter, check if value matches
                        actual_value = str(item_series_args[param_name])
                        if actual_value not in expected_values:
                            series_match = False
                            break
                    # If item doesn't have this parameter, it passes the filter

            if not series_match:
                continue

            # Создаем ключ группировки (convert dicts to strings to make them hashable)
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

        # Выбираем по одному представителю из каждой группы (лучший по минимальной ошибке)
        filtered = []
        for group_items in grouped_data.values():
            # Находим элемент с минимальной финальной ошибкой
            best_item = min(group_items, key=lambda x: self.get_final_error(x))
            filtered.append(best_item)

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

    # Загружаем данные один раз при старте сервера
    with open(data_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    # Preprocess data to handle complex numbers and scientific notation
    data = preprocess_data(data)

    print(f"Loaded {len(data)} records from {data_file}")

    # Создаем обработчик с предзагруженными данными
    def handler(*args, **kwargs):
        return DataAPIHandler(*args, data=data, **kwargs)

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
