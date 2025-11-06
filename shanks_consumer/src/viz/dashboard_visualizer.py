import json
import pathlib
from typing import Any, Dict, List

import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots


class DashboardVisualizer:
    def __init__(self):
        self.data = None
        self.series_names = []
        self.stack_ids = []
        self.method_names = []

    def load_data(self, data_source: pathlib.Path | List[Dict]):
        """Загружает данные из файла или принимает готовый список"""
        if isinstance(data_source, pathlib.Path):
            with open(data_source, 'r', encoding='utf-8') as f:
                self.data = json.load(f)
        else:
            self.data = data_source
        
        self._extract_metadata()
    
    def _extract_metadata(self):
        """Извлекает уникальные значения для фильтров"""
        if not self.data:
            return
        
        series_names = set()
        stack_ids = set()
        method_names = set()
        
        for item in self.data:
            if 'series' in item and 'name' in item['series']:
                series_names.add(item['series']['name'])
            if 'stack_id' in item:
                stack_ids.add(item['stack_id'])
            if 'accel' in item and 'name' in item['accel']:
                method_names.add(item['accel']['name'])
        
        self.series_names = sorted(list(series_names))
        self.stack_ids = sorted(list(stack_ids))
        self.method_names = sorted(list(method_names))
    
    def filter_data(self, series_name: str, selected_stack_ids: List[str] | None = None, 
                   selected_methods: List[str] | None = None, m_values: List[int] | None = None) -> List[Dict]:
        """Фильтрует данные по заданным критериям"""
        if not self.data:
            return []
        
        filtered = []
        for item in self.data:
            # Фильтр по series name
            if 'series' not in item or item['series'].get('name') != series_name:
                continue
            
            # Фильтр по stack_id
            if selected_stack_ids and item.get('stack_id') not in selected_stack_ids:
                continue
            
            # Фильтр по методу
            if selected_methods and 'accel' in item and item['accel'].get('name') not in selected_methods:
                continue
            
            # Фильтр по m_value
            if m_values is not None and 'accel' in item and item['accel'].get('m_value') not in m_values:
                continue
            
            # Добавляем только если есть вычисленные данные
            if item.get('computed') and len(item['computed']) > 0:
                filtered.append(item)
        
        return filtered
    
    def create_interactive_dashboard(self, series_name: str | None = None, output_file: pathlib.Path | None = None):
        """Создает интерактивный дашборд для выбранного ряда или для всех рядов"""
        
        # Получаем все доступные stack_id для этого ряда
        if not self.data:
            series_data = []
        else:
            series_data = [item for item in self.data if 'series' in item and item['series'].get('name') == series_name]
        
        available_stack_ids = [item.get('stack_id') for item in series_data if item.get('stack_id')]
        available_stack_ids = list(set(available_stack_ids))
        available_methods = [item['accel'].get('name') for item in series_data if 'accel' in item and item['accel'].get('name')]
        available_methods = list(set(available_methods))
        available_m_values = [item['accel'].get('m_value') for item in series_data if 'accel' in item and item['accel'].get('m_value') is not None]
        available_m_values = sorted(list(set(available_m_values)))
        
        if series_name and output_file:
            # Преобразуем в правильные типы
            stack_ids = [sid for sid in available_stack_ids if sid is not None]
            methods = [m for m in available_methods if m is not None]
            m_values = [m for m in available_m_values if m is not None]
            self._create_single_series_dashboard(series_name, series_data, stack_ids, methods, m_values, output_file)
        elif output_file:
            self._create_all_series_dashboard(output_file)
    
    def _create_single_series_dashboard(self, series_name: str, series_data: List[Dict], 
                                     available_stack_ids: List[str], available_methods: List[str], 
                                     available_m_values: List[int], output_file: pathlib.Path):
        """Создает дашборд для одного ряда"""
        
        # Создаем HTML с интерактивными элементами
        html_template = f"""
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Дашборд для ряда: {series_name}</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #1e1e1e;
            color: #ffffff;
        }}
        .container {{
            max-width: 1400px;
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
        select {{
            width: 100%;
            padding: 8px;
            background-color: #3d3d3d;
            color: #ffffff;
            border: 1px solid #555;
            border-radius: 4px;
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
        <h1>Интерактивный дашборд для ряда: {series_name}</h1>
        
        <div class="filters">
            <h2>Фильтры</h2>
            
            <div class="filter-group">
                <label>Stack ID (выберите для отображения):</label>
                <div class="checkbox-group" id="stack-filters">
                    {self._create_checkboxes(available_stack_ids, 'stack')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Методы ускорения:</label>
                <div class="checkbox-group" id="method-filters">
                    {self._create_checkboxes(available_methods, 'method')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Значения m:</label>
                <div class="checkbox-group" id="mvalue-filters">
                    {self._create_checkboxes(available_m_values, 'mvalue')}
                </div>
            </div>
            
            <button class="update-btn" onclick="updatePlots()">Обновить графики</button>
        </div>
        
        <div class="stats" id="stats">
            <h3>Статистика</h3>
            <p>Загрузка данных...</p>
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
        // Данные для визуализации
        const allData = {json.dumps(series_data)};
        let currentData = allData;
        
        // Инициализация - выбираем все чекбоксы по умолчанию
        document.addEventListener('DOMContentLoaded', function() {{
            selectAllCheckboxes('stack');
            selectAllCheckboxes('method');
            selectAllCheckboxes('mvalue');
            updatePlots();
        }});
        
        function selectAllCheckboxes(type) {{
            const checkboxes = document.querySelectorAll(`input[name="${{type}}"]`);
            checkboxes.forEach(cb => cb.checked = true);
        }}
        
        function getSelectedValues(type) {{
            const checkboxes = document.querySelectorAll(`input[name="${{type}}"]:checked`);
            return Array.from(checkboxes).map(cb => cb.value);
        }}
        
        function filterData() {{
            const selectedStacks = getSelectedValues('stack');
            const selectedMethods = getSelectedValues('method');
            const selectedMValues = getSelectedValues('mvalue').map(v => parseInt(v));
            
            currentData = allData.filter(item => {{
                // Фильтр по stack_id
                if (selectedStacks.length > 0 && !selectedStacks.includes(item.stack_id)) {{
                    return false;
                }}
                
                // Фильтр по методу
                if (selectedMethods.length > 0 && item.accel && !selectedMethods.includes(item.accel.name)) {{
                    return false;
                }}
                
                // Фильтр по m_value
                if (selectedMValues.length > 0 && item.accel && !selectedMValues.includes(item.accel.m_value)) {{
                    return false;
                }}
                
                // Только с вычисленными данными
                return item.computed && item.computed.length > 0;
            }});
            
            updateStats();
        }}
        
        function updateStats() {{
            const statsDiv = document.getElementById('stats');
            const totalItems = currentData.length;
            const uniqueStacks = [...new Set(currentData.map(item => item.stack_id))].length;
            const uniqueMethods = [...new Set(currentData.map(item => item.accel?.name))].length;
            
            statsDiv.innerHTML = `
                <h3>Статистика</h3>
                <p>Всего записей: ${{totalItems}}</p>
                <p>Уникальных stack_id: ${{uniqueStacks}}</p>
                <p>Уникальных методов: ${{uniqueMethods}}</p>
            `;
        }}
        
        function updatePlots() {{
            filterData();
            
            if (currentData.length === 0) {{
                document.getElementById('convergence-plot').innerHTML = '<p>Нет данных для отображения</p>';
                document.getElementById('error-plot').innerHTML = '<p>Нет данных для отображения</p>';
                document.getElementById('performance-plot').innerHTML = '<p>Нет данных для отображения</p>';
                return;
            }}
            
            createConvergencePlot();
            createErrorPlot();
            createPerformancePlot();
        }}
        
        function createConvergencePlot() {{
            const traces = [];
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const computed = item.computed;
                const variation = `(m=${{item.accel.m_value}}) ${{JSON.stringify(item.accel.additional_args)}} [${{JSON.stringify(item.series.arguments)}}]`;
                
                // Основная линия - ускоренные значения
                traces.push({{
                    x: computed.map(c => c.n),
                    y: computed.map(c => c.accel_value),
                    mode: 'lines+markers',
                    name: variation,
                    hovertemplate: 'Итерация: %{{x}}<br>Ускоренное значение: %{{y}}:.10f<extra></extra>',
                    line: {{ width: 2 }}
                }});
                
                // Пунктирная линия - частичные суммы
                traces.push({{
                    x: computed.map(c => c.n),
                    y: computed.map(c => c.partial_sum),
                    mode: 'lines+markers',
                    name: variation + ' (частичные суммы)',
                    hovertemplate: 'Итерация: %{{x}}<br>Частичная сумма: %{{y}}:.10f<extra></extra>',
                    line: {{ dash: 'dash', width: 1 }},
                    marker: {{ size: 4, symbol: 'x' }}
                }});
            }});
            
            const layout = {{
                title: 'Сходимость методов',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Значение' }},
                template: 'plotly_dark',
                height: 600
            }};
            
            Plotly.newPlot('convergence-plot', traces, layout);
        }}
        
        function createErrorPlot() {{
            const traces = [];
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const computed = item.computed;
                const variation = `(m=${{item.accel.m_value}}) ${{item.accel.name}}`;
                
                const errors = computed.map(c => Math.abs(c.accel_value_deviation || 0));
                
                traces.push({{
                    x: computed.map(c => c.n),
                    y: errors,
                    mode: 'lines+markers',
                    name: variation,
                    hovertemplate: 'Итерация: %{{x}}<br>Ошибка: %{{y}}:.2e<extra></extra>',
                    line: {{ width: 2 }}
                }});
            }});
            
            const layout = {{
                title: 'Ошибка сходимости (логарифмическая шкала)',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Абсолютная ошибка', type: 'log' }},
                template: 'plotly_dark',
                height: 600
            }};
            
            Plotly.newPlot('error-plot', traces, layout);
        }}
        
        function createPerformancePlot() {{
            const performanceData = {{}};
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const method = item.accel.name;
                const mValue = item.accel.m_value;
                const key = `${{method}} (m=${{mValue}})`;
                
                if (!performanceData[key]) {{
                    performanceData[key] = [];
                }}
                
                // Находим минимальную ошибку для оценки производительности
                const errors = item.computed.map(c => Math.abs(c.accel_value_deviation || 0));
                const minError = Math.min(...errors);
                const iterationAtMinError = item.computed.find(c => Math.abs(c.accel_value_deviation || 0) === minError)?.n || 0;
                
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
                    text: data.map(d => `Stack: ${{d.stackId}}<br>Финальная ошибка: ${{d.finalError.toExponential(2)}}`),
                    hovertemplate: '<b>%{{fullData.name}}</b><br>%{{text}}<br>Итерация мин. ошибки: %{{x}}<br>Мин. ошибка: %{{y}}:.2e<extra></extra>',
                    marker: {{ size: 10 }}
                }});
            }});
            
            const layout = {{
                title: 'Производительность методов',
                xaxis: {{ title: 'Итерация достижения минимальной ошибки' }},
                yaxis: {{ title: 'Минимальная ошибка', type: 'log' }},
                template: 'plotly_dark',
                height: 600
            }};
            
            Plotly.newPlot('performance-plot', traces, layout);
        }}
    </script>
</body>
</html>
        """
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(html_template)
    
    def _create_checkboxes(self, items: List, name_prefix: str) -> str:
        """Создает HTML для чекбоксов"""
        checkboxes = []
        for item in items:
            checkboxes.append(f"""
                <div class="checkbox-item">
                    <input type="checkbox" id="{name_prefix}_{item}" name="{name_prefix}" value="{item}" checked>
                    <label for="{name_prefix}_{item}">{item}</label>
                </div>
            """)
        return ''.join(checkboxes)
    
    def _create_all_series_dashboard(self, output_file: pathlib.Path):
        """Создает универсальный дашборд для всех рядов"""
        
        # Создаем HTML с интерактивными элементами
        html_template = f"""
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Универсальный дашборд для всех рядов</title>
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
        select {{
            width: 100%;
            padding: 8px;
            background-color: #3d3d3d;
            color: #ffffff;
            border: 1px solid #555;
            border-radius: 4px;
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
        .stats {{
            background-color: #2d2d2d;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
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
        .checkbox-item {{
            background-color: #3d3d3d;
            padding: 8px;
            border-radius: 4px;
            border: 1px solid #555;
            transition: opacity 0.3s ease, color 0.3s ease;
        }}
        .checkbox-item input {{
            margin-right: 8px;
        }}
        .checkbox-item input:disabled {{
            cursor: not-allowed;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Универсальный дашборд для всех рядов</h1>
        
        <div class="series-selector">
            <h3>Выберите ряд(ы) для анализа:</h3>
            <div id="series-buttons">
                {self._create_series_buttons()}
            </div>
        </div>
        
        <div class="filters">
            <h2>Фильтры</h2>
            
            <div class="filter-group">
                <label>Stack ID (выберите для отображения):</label>
                <div class="checkbox-group" id="stack-filters">
                    {self._create_checkboxes(self.stack_ids, 'stack')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Методы ускорения:</label>
                <div class="checkbox-group" id="method-filters">
                    {self._create_checkboxes(self.method_names, 'method')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Значения m:</label>
                <div class="checkbox-group" id="mvalue-filters">
                    {self._create_mvalue_checkboxes()}
                </div>
            </div>
            
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
        // Данные для визуализации
        const allData = {json.dumps(self.data)};
        let currentData = [];
        let selectedSeries = new Set();
        
        // Инициализация
        document.addEventListener('DOMContentLoaded', function() {{
            selectAllCheckboxes('stack');
            selectAllCheckboxes('method');
            selectAllCheckboxes('mvalue');
            setupSeriesButtons();
            updateStackIdAvailability();
        }});
        
        function setupSeriesButtons() {{
            const buttons = document.querySelectorAll('.series-btn');
            buttons.forEach(btn => {{
                btn.addEventListener('click', function() {{
                    const seriesName = this.getAttribute('data-series');
                    if (selectedSeries.has(seriesName)) {{
                        selectedSeries.delete(seriesName);
                        this.classList.remove('active');
                    }} else {{
                        selectedSeries.add(seriesName);
                        this.classList.add('active');
                    }}
                    // Обновляем доступность Stack ID при изменении выбранных рядов
                    updateStackIdAvailability();
                }});
            }});
        }}
        
        function selectAllCheckboxes(type) {{
            const checkboxes = document.querySelectorAll(`input[name="${{type}}"]`);
            checkboxes.forEach(cb => cb.checked = true);
        }}
        
        function getSelectedValues(type) {{
            const checkboxes = document.querySelectorAll(`input[name="${{type}}"]:checked`);
            return Array.from(checkboxes).map(cb => cb.value);
        }}
        
        function filterData() {{
            if (selectedSeries.size === 0) {{
                currentData = [];
                return;
            }}
            
            const selectedStacks = getSelectedValues('stack');
            const selectedMethods = getSelectedValues('method');
            const selectedMValues = getSelectedValues('mvalue').map(v => parseInt(v));
            
            currentData = allData.filter(item => {{
                // Фильтр по рядам
                if (!selectedSeries.has(item.series?.name)) {{
                    return false;
                }}
                
                // Фильтр по stack_id
                if (selectedStacks.length > 0 && !selectedStacks.includes(item.stack_id)) {{
                    return false;
                }}
                
                // Фильтр по методу
                if (selectedMethods.length > 0 && item.accel && !selectedMethods.includes(item.accel.name)) {{
                    return false;
                }}
                
                // Фильтр по m_value
                if (selectedMValues.length > 0 && item.accel && !selectedMValues.includes(item.accel.m_value)) {{
                    return false;
                }}
                
                // Только с вычисленными данными
                return item.computed && item.computed.length > 0;
            }});
            
            updateStats();
        }}
        
        function updateStats() {{
            const statsDiv = document.getElementById('stats');
            const totalItems = currentData.length;
            const uniqueSeries = [...new Set(currentData.map(item => item.series?.name))].length;
            const uniqueStacks = [...new Set(currentData.map(item => item.stack_id))].length;
            const uniqueMethods = [...new Set(currentData.map(item => item.accel?.name))].length;
            
            statsDiv.innerHTML = `
                <h3>Статистика</h3>
                <p>Выбранные ряды: ${{Array.from(selectedSeries).join(', ')}}</p>
                <p>Всего записей: ${{totalItems}}</p>
                <p>Уникальных рядов: ${{uniqueSeries}}</p>
                <p>Уникальных stack_id: ${{uniqueStacks}}</p>
                <p>Уникальных методов: ${{uniqueMethods}}</p>
            `;
        }}
        
        function updateStackIdAvailability() {{
            // Получаем все Stack ID checkboxes
            const stackCheckboxes = document.querySelectorAll('input[name="stack"]');
            
            if (selectedSeries.size === 0) {{
                // Если ряды не выбраны, все Stack ID доступны
                stackCheckboxes.forEach(checkbox => {{
                    checkbox.disabled = false;
                    checkbox.parentElement.style.opacity = '1';
                    checkbox.parentElement.style.color = '#ffffff';
                }});
                return;
            }}
            
            // Получаем Stack ID, которые содержат выбранные ряды
            const availableStackIds = new Set();
            allData.forEach(item => {{
                if (selectedSeries.has(item.series?.name)) {{
                    availableStackIds.add(item.stack_id);
                }}
            }});
            
            // Обновляем состояние каждого Stack ID checkbox
            stackCheckboxes.forEach(checkbox => {{
                const stackId = checkbox.value;
                const isAvailable = availableStackIds.has(stackId);
                
                checkbox.disabled = !isAvailable;
                if (isAvailable) {{
                    checkbox.parentElement.style.opacity = '1';
                    checkbox.parentElement.style.color = '#ffffff';
                }} else {{
                    checkbox.parentElement.style.opacity = '0.4';
                    checkbox.parentElement.style.color = '#888888';
                }}
            }});
        }}
        
        function updatePlots() {{
            filterData();
            
            if (currentData.length === 0) {{
                document.getElementById('convergence-plot').innerHTML = '<p>Нет данных для отображения. Выберите ряды и проверьте фильтры.</p>';
                document.getElementById('error-plot').innerHTML = '<p>Нет данных для отображения</p>';
                document.getElementById('performance-plot').innerHTML = '<p>Нет данных для отображения</p>';
                return;
            }}
            
            createConvergencePlot();
            createErrorPlot();
            createPerformancePlot();
        }}
        
        function createConvergencePlot() {{
            const traces = [];
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const computed = item.computed;
                const variation = `${{item.series.name}} (m=${{item.accel.m_value}}) ${{item.accel.name}}`;
                
                // Основная линия - ускоренные значения
                traces.push({{
                    x: computed.map(c => c.n),
                    y: computed.map(c => c.accel_value),
                    mode: 'lines+markers',
                    name: variation,
                    hovertemplate: '<b>%{{fullData.name}}</b><br>Ряд: ${{item.series.name}}<br>Итерация: %{{x}}<br>Ускоренное значение: %{{y}}:.10f<extra></extra>',
                    line: {{ width: 2 }}
                }});
                
                // Пунктирная линия - частичные суммы
                traces.push({{
                    x: computed.map(c => c.n),
                    y: computed.map(c => c.partial_sum),
                    mode: 'lines+markers',
                    name: variation + ' (частичные суммы)',
                    hovertemplate: '<b>%{{fullData.name}}</b><br>Ряд: ${{item.series.name}}<br>Итерация: %{{x}}<br>Частичная сумма: %{{y}}:.10f<extra></extra>',
                    line: {{ dash: 'dash', width: 1 }},
                    marker: {{ size: 4, symbol: 'x' }}
                }});
            }});
            
            const layout = {{
                title: 'Сходимость методов (все ряды)',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Значение' }},
                template: 'plotly_dark',
                height: 700,
                showlegend: true
            }};
            
            Plotly.newPlot('convergence-plot', traces, layout);
        }}
        
        function createErrorPlot() {{
            const traces = [];
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const computed = item.computed;
                const variation = `${{item.series.name}} (m=${{item.accel.m_value}}) ${{item.accel.name}}`;
                
                const errors = computed.map(c => Math.abs(c.accel_value_deviation || 0));
                
                traces.push({{
                    x: computed.map(c => c.n),
                    y: errors,
                    mode: 'lines+markers',
                    name: variation,
                    hovertemplate: '<b>%{{fullData.name}}</b><br>Ряд: ${{item.series.name}}<br>Итерация: %{{x}}<br>Ошибка: %{{y}}:.2e<extra></extra>',
                    line: {{ width: 2 }}
                }});
            }});
            
            const layout = {{
                title: 'Ошибка сходимости (логарифмическая шкала)',
                xaxis: {{ title: 'Итерация n' }},
                yaxis: {{ title: 'Абсолютная ошибка', type: 'log' }},
                template: 'plotly_dark',
                height: 700
            }};
            
            Plotly.newPlot('error-plot', traces, layout);
        }}
        
        function createPerformancePlot() {{
            const performanceData = {{}};
            
            currentData.forEach(item => {{
                if (!item.computed || item.computed.length === 0) return;
                
                const series = item.series.name;
                const method = item.accel.name;
                const mValue = item.accel.m_value;
                const key = `${{series}} - ${{method}} (m=${{mValue}})`;
                
                if (!performanceData[key]) {{
                    performanceData[key] = [];
                }}
                
                // Находим минимальную ошибку для оценки производительности
                const errors = item.computed.map(c => Math.abs(c.accel_value_deviation || 0));
                const minError = Math.min(...errors);
                const iterationAtMinError = item.computed.find(c => Math.abs(c.accel_value_deviation || 0) === minError)?.n || 0;
                
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
                    text: data.map(d => `Stack: ${{d.stackId}}<br>Финальная ошибка: ${{d.finalError.toExponential(2)}}`),
                    hovertemplate: '<b>%{{fullData.name}}</b><br>%{{text}}<br>Итерация мин. ошибки: %{{x}}<br>Мин. ошибка: %{{y}}:.2e<extra></extra>',
                    marker: {{ size: 10 }}
                }});
            }});
            
            const layout = {{
                title: 'Производительность методов (все ряды)',
                xaxis: {{ title: 'Итерация достижения минимальной ошибки' }},
                yaxis: {{ title: 'Минимальная ошибка', type: 'log' }},
                template: 'plotly_dark',
                height: 700
            }};
            
            Plotly.newPlot('performance-plot', traces, layout);
        }}
    </script>
</body>
</html>
        """
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(html_template)
    
    def _create_series_buttons(self) -> str:
        """Создает кнопки для выбора рядов"""
        buttons = []
        for series_name in self.series_names:
            buttons.append(f"""
                <button class="series-btn" data-series="{series_name}">{series_name}</button>
            """)
        return ''.join(buttons)
    
    def _create_mvalue_checkboxes(self) -> str:
        """Создает чекбоксы для значений m"""
        # Получаем все уникальные значения m из данных
        m_values = set()
        if self.data:
            for item in self.data:
                if 'accel' in item and item['accel'].get('m_value') is not None:
                    m_values.add(item['accel']['m_value'])
        
        sorted_m_values = sorted(list(m_values))
        checkboxes = []
        for m_value in sorted_m_values:
            checkboxes.append(f"""
                <div class="checkbox-item">
                    <input type="checkbox" id="mvalue_{m_value}" name="mvalue" value="{m_value}" checked>
                    <label for="mvalue_{m_value}">{m_value}</label>
                </div>
            """)
        return ''.join(checkboxes)