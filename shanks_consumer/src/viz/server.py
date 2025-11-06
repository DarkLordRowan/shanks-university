import json
import urllib.parse
from http.server import HTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
from typing import Dict, List, Set
import argparse


class DataAPIHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, data=None, **kwargs):
        self.data = data  # Pre-loaded data passed at server startup
        super().__init__(*args, **kwargs)
    
    def get_data(self):
        """Возвращает предзагруженные данные"""
        return self.data
    
    def do_GET(self):
        if self.path.startswith('/api/data'):
            self.handle_api_request()
        else:
            # Обслуживаем статические файлы
            if self.path == '/':
                self.path = '/index.html'
            return super().do_GET()
    
    def handle_api_request(self):
        """Обрабатывает API запросы"""
        try:
            print(f"API request received: {self.path}")
            # Парсим query параметры
            parsed_url = urllib.parse.urlparse(self.path)
            query_params = urllib.parse.parse_qs(parsed_url.query)
            
            # Извлекаем фильтры
            series_filter = query_params.get('series', [])
            stack_ids_filter = query_params.get('stack_id', [])
            methods_filter = query_params.get('method', [])
            m_values_filter = query_params.get('m_value', [])
            
            # Конвертируем строки в нужные типы
            m_values_int = []
            if m_values_filter:
                m_values_int = [int(v) for v in m_values_filter]
            
            # Фильтруем данные
            filtered_data = self.filter_data(
                series_filter, stack_ids_filter, methods_filter, m_values_int
            )
            
            # Отправляем ответ
            self.send_json_response(filtered_data)
            
        except Exception as e:
            self.send_error(500, f"Server error: {str(e)}")
    
    def filter_data(self, series_filter: List[str], stack_ids_filter: List[str], 
                   methods_filter: List[str], m_values_filter: List[int]) -> List[Dict]:
        """Фильтрует данные по заданным критериям"""
        data = self.get_data()
        filtered = []
        
        for item in data:
            # Пропускаем записи без вычисленных данных
            if not item.get('computed') or len(item['computed']) == 0:
                continue
            
            # Фильтр по рядам
            if series_filter and item.get('series', {}).get('name') not in series_filter:
                continue
            
            # Фильтр по stack_id
            if stack_ids_filter and item.get('stack_id') not in stack_ids_filter:
                continue
            
            # Фильтр по методам
            if methods_filter and item.get('accel', {}).get('name') not in methods_filter:
                continue
            
            # Фильтр по m_value
            if m_values_filter and item.get('accel', {}).get('m_value') not in m_values_filter:
                continue
            
            filtered.append(item)
        
        return filtered
    
    def send_json_response(self, data):
        """Отправляет JSON ответ"""
        response = json.dumps(data)
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.send_header('Content-length', str(len(response)))
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(response.encode('utf-8'))


def _create_series_buttons(series_names: List[str]) -> str:
    """Создает кнопки для выбора рядов"""
    buttons = []
    for series_name in series_names:
        buttons.append(f"""
            <button class="series-btn" data-series="{series_name}">{series_name}</button>
        """)
    return ''.join(buttons)

def _create_checkboxes(items: List, name_prefix: str) -> str:
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

def create_server_dashboard(data_file: Path, port: int = 8000):
    """Создает HTML файл для дашборда с API"""
    
    # Загружаем данные для получения метаданных
    with open(data_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # Извлекаем уникальные значения
    series_names = set()
    stack_ids = set()
    method_names = set()
    m_values = set()
    
    for item in data:
        if item.get('series', {}).get('name'):
            series_names.add(item['series']['name'])
        if item.get('stack_id'):
            stack_ids.add(item['stack_id'])
        if item.get('accel', {}).get('name'):
            method_names.add(item['accel']['name'])
        if item.get('accel', {}).get('m_value') is not None:
            m_values.add(item['accel']['m_value'])
    
    series_names = sorted(list(series_names))
    stack_ids = sorted(list(stack_ids))
    method_names = sorted(list(method_names))
    m_values = sorted(list(m_values))
    
    # Создаем HTML с API интеграцией
    html_content = f"""
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Интерактивный дашборд (API режим)</title>
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
            transition: opacity 0.3s ease, color 0.3s ease;
        }}
        .checkbox-item input {{
            margin-right: 8px;
        }}
        .checkbox-item input:disabled {{
            cursor: not-allowed;
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
        .update-btn:disabled {{
            background-color: #666;
            cursor: not-allowed;
        }}
        .stats {{
            background-color: #2d2d2d;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
        }}
        .loading {{
            color: #007acc;
            font-style: italic;
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
    </style>
</head>
<body>
    <div class="container">
        <h1>Интерактивный дашборд (API режим)</h1>
        
        <div class="series-selector">
            <h3>Выберите ряд(ы) для анализа:</h3>
            <div id="series-buttons">
                {_create_series_buttons(series_names)}
            </div>
        </div>
        
        <div class="filters">
            <h2>Фильтры</h2>
            
            <div class="filter-group">
                <label>Stack ID (выберите для отображения):</label>
                <div class="checkbox-group" id="stack-filters">
                    {_create_checkboxes(stack_ids, 'stack')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Методы ускорения:</label>
                <div class="checkbox-group" id="method-filters">
                    {_create_checkboxes(method_names, 'method')}
                </div>
            </div>
            
            <div class="filter-group">
                <label>Значения m:</label>
                <div class="checkbox-group" id="mvalue-filters">
                    {_create_checkboxes(m_values, 'mvalue')}
                </div>
            </div>
            
            <button class="update-btn" id="update-btn" onclick="updatePlots()">Обновить графики</button>
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
        let selectedSeries = new Set();
        let isLoading = false;
        
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
        
        async function loadDataFromAPI() {{
            if (isLoading) return;
            
            isLoading = true;
            const updateBtn = document.getElementById('update-btn');
            updateBtn.disabled = true;
            updateBtn.textContent = 'Загрузка...';
            
            try {{
                // Строим URL с параметрами
                const params = new URLSearchParams();
                
                // Добавляем выбранные ряды
                selectedSeries.forEach(series => params.append('series', series));
                
                // Добавляем выбранные фильтры
                getSelectedValues('stack').forEach(stack => params.append('stack_id', stack));
                getSelectedValues('method').forEach(method => params.append('method', method));
                getSelectedValues('mvalue').forEach(m => params.append('m_value', m));
                
                const response = await fetch(`/api/data?${{params.toString()}}`);
                if (!response.ok) {{
                    throw new Error(`HTTP error! status: ${{response.status}}`);
                }}
                
                currentData = await response.json();
                updateStats();
                createPlots();
                
            }} catch (error) {{
                console.error('Error loading data:', error);
                document.getElementById('stats').innerHTML = `
                    <h3>Ошибка</h3>
                    <p class="loading">Не удалось загрузить данные: ${{error.message}}</p>
                `;
            }} finally {{
                isLoading = false;
                updateBtn.disabled = false;
                updateBtn.textContent = 'Обновить графики';
            }}
        }}
        
        function updatePlots() {{
            if (selectedSeries.size === 0) {{
                document.getElementById('stats').innerHTML = `
                    <h3>Статистика</h3>
                    <p>Выберите хотя бы один ряд для анализа</p>
                `;
                clearPlots();
                return;
            }}
            
            loadDataFromAPI();
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
        
        function createPlots() {{
            if (currentData.length === 0) {{
                clearPlots();
                return;
            }}
            
            createConvergencePlot();
            createErrorPlot();
            createPerformancePlot();
        }}
        
        function clearPlots() {{
            document.getElementById('convergence-plot').innerHTML = '<p>Нет данных для отображения</p>';
            document.getElementById('error-plot').innerHTML = '<p>Нет данных для отображения</p>';
            document.getElementById('performance-plot').innerHTML = '<p>Нет данных для отображения</p>';
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
        
        async function updateStackIdAvailability() {{
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
            
            try {{
                // Запрашиваем данные для определения доступных Stack ID
                const params = new URLSearchParams();
                selectedSeries.forEach(series => params.append('series', series));
                
                const response = await fetch(`/api/data?${{params.toString()}}`);
                const data = await response.json();
                
                // Получаем Stack ID, которые содержат выбранные ряды
                const availableStackIds = new Set(data.map(item => item.stack_id));
                
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
                
            }} catch (error) {{
                console.error('Error updating Stack ID availability:', error);
            }}
        }}
    </script>
</body>
</html>
    """
    
    # Сохраняем HTML файл
    output_dir = Path('output')
    output_dir.mkdir(exist_ok=True)
    
    with open(output_dir / 'index.html', 'w', encoding='utf-8') as f:
        f.write(html_content)
    
    return output_dir / 'index.html'



def create_handler_class(data_file: Path):
    """Создает класс обработчика с привязанным файлом данных"""
    
    # Load data once at server startup
    print(f"Loading data from {data_file}...")
    with open(data_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    print(f"Loaded {len(data)} records")
    
    class Handler(DataAPIHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, data=data, **kwargs)
    
    return Handler


def start_server(data_file: Path, port: int = 8000):
    """Запускает HTTP сервер с дашбордом"""
    
    # Создаем HTML дашборда
    dashboard_file = create_server_dashboard(data_file, port)
    print(f"Dashboard created: {dashboard_file}")
    
    # Создаем класс обработчика с предзагруженными данными
    handler_class = create_handler_class(data_file)
    
    # Запускаем сервер из output директории
    output_dir = Path('output')
    if not output_dir.exists():
        output_dir.mkdir(exist_ok=True)
    
    # Меняем рабочую директорию на output для обслуживания статических файлов
    import os
    original_cwd = os.getcwd()
    try:
        os.chdir(output_dir)
        
        # Запускаем сервер
        server_address = ('', port)
        httpd = HTTPServer(server_address, handler_class)
        
        print(f"\\n🚀 Server started!")
        print(f"📊 Dashboard: http://localhost:{port}")
        print(f"📁 Serving from: {Path.cwd()}")
        print(f"📄 Data file: {data_file}")
        print(f"\\nPress Ctrl+C to stop the server\\n")
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\\n👋 Server stopped")
            httpd.server_close()
    finally:
        os.chdir(original_cwd)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Start dashboard server')
    parser.add_argument('--data-file', required=True, type=Path, help='JSON data file')
    parser.add_argument('--port', type=int, default=8000, help='Server port (default: 8000)')
    
    args = parser.parse_args()
    
    if not args.data_file.exists():
        print(f"Error: Data file {args.data_file} not found")
        exit(1)
    
    start_server(args.data_file, args.port)