# Поток данных (Dataflow)

## Обзор

Документ описывает полный жизненный цикл данных в фреймворке - от загрузки конфигурации до экспорта результатов и визуализации.

## Пошаговый поток данных

### Шаг 1. Инициализация (CLI)

**Файл:** `cli.py`

```bash
python3 run --config backend/consumer/config/options.json
```

---

### Шаг 2. Загрузка конфигурации

**Файл:** `config/model.py`

```python
cfg = TrialConfig.load(Path("config/options.json"))
```

**Загружаемые поля:**

| Поле | Описание | По умолчанию |
|------|----------|--------------|
| `verbose` | Уровень логирования | 0 |
| `series_json` | Путь к JSON с рядами | `config/example.json` |
| `accel_json` | Путь к JSON с методами ускорения | `config/example.json` |
| `noise_json` | Путь к JSON с шумами | `config/example.json` |
| `filters_json` | Путь к JSON с фильтрами | `config/example.json` |
| `output_dir` | Директория вывода | `output` |
| `results_filename` | Имя файла результатов | `results` |
| `trial_process_count` | Число параллельных процессов | 1 |
| `trial_task_timeout` | Таймаут задачи (сек) | 10 |
| `trial_memory_efficient` | Режим экономии памяти | true |
| `precisions` | Типы точности | `[F64]` |
| `output_formats` | Форматы вывода | `[JSON, CSV]` |

**Дополнительно загружается:**
- `noises` — список конфигураций шума из `noise_json`
- `filters` — список конфигураций фильтров из `filters_json`

---

### Шаг 3. Dependency Injection

**Файл:** `dependency.py`

```python
executor = get_trial_executor_from_config(cfg)
```

**Создаётся:**

1. **TrialRunner** — `ParallelTrialRunner` или `SequentialTrialRunner` в зависимости от `trial_process_count`
2. **SeriesParamSource'ы** — из `PARAM_SERIES_SOURCE_BUILDERS` registry:
   - `JSONSeriesParamSource`
   - `CSVSeriesParamSource`
3. **AccelParamSource'ы** — из `PARAM_ACCELS_SOURCE_BUILDERS` registry:
   - `JSONAccelParamSource` — если `accel_json` существует
4. **Serializer** — `TrialResultSerializer`
5. **Exporters** — из `EXPORT_BUILDERS` registry для каждого формата в `output_formats`

**Результат:** Fully configured `TrialExecutor`

---

### Шаг 4. Загрузка параметров

**Файл:** `services/trial_executor.py:load_parameters()`

```python
series_params, accel_params = executor.load_parameters(PrecisionType.F64)
```

**Процесс:**

1. Для каждого `SeriesParamSource` вызывается `load(precision)`
2. Для каждого `AccelParamSource` вызывается `load(precision)`
3. Источники также загружают `noise_configs`

**Из JSON файла загружаются:**

#### Series параметры
```json
{
  "series": [
    {
      "name": "CosSeries",
      "args": {"x": [1, 2]}
    }
  ]
}
```

#### Accel параметры
```json
{
  "methods": [
    {
      "name": "LevinAlgorithm",
      "n": {"start": 10, "stop": 100, "step": 10},
      "m": [8],
      "args": {
        "remainder": ["v_type"],
        "useRecurrentFormula": true
      },
      "events": [...]
    }
  ]
}
```

**Функция `autowrap`:**
- Единичное значение -> одно значение
- Список -> каждое значение отдельно
- Объект `{start, stop, step}` -> генерирует диапазон

**Результат:**
- `series_params: list[BaseSeriesParam]`
- `accel_params: list[BaseAccelParam]`

---

### Шаг 5. Генерация комбинаций

**Файл:** `domain/complex_trial.py`

```python
trial = ComplexTrial(series_params, accel_params)
combinations = trial.combinations()
```

**Декартово произведение:**
```
series_params * accel_params = [
    (series_1, accel_1),
    (series_1, accel_2),
    ...
    (series_N, accel_M)
]
```

**Каждая комбинация будет выполнена со всеми:**
- `noise_configs` (если есть)
- `filter_configs` (если есть)
- `precisions` (итерация внешнего цикла)

**Результат:** `list[tuple[BaseSeriesParam, BaseAccelParam]]`

---

### Шаг 6. Выполнение trial

**Файл:** `domain/use_cases/run_trial.py:execute_trial()`

```python
results = execute_trial(
    (series, accel),
    noise_config,
    filter_configs
)
```

**Подпроцессы:**

#### 6.1 Получение ряда
```python
series_result, series_lim = series.obtain_by_argument(series_argument, size_floor)
```
- Создаётся инстанс C++ ряда (например `CosSeriesF64`)
- Генерируются значения ряда до `size_floor`
- Вычисляется теоретический предел `series_lim`

#### 6.2 Применение шума (опционально)
```python
if noise_config:
    func_name = f"applyNoise{precision.value}"
    series_result = getattr(ps, func_name)(
        series_result,
        noise_method_enum,
        noise_type_enum,
        seed, param1, param2
    )
```

#### 6.3 Создание инстанса алгоритма
```python
accel_instance = accel.create_instance(additional_args)
```
- Создаётся C++ инстанс алгоритма (например `LevinAlgorithmF64`)

#### 6.4 Итерация по n и m
```python
for m_value in m_values:
    ctx = accel.create_event_context()
    for n_value in n_values:
        partial_sum = series_result.Sn[n_value - 1]
        accel_value = accel_instance(n_value, m_value, series_result)

        computed.append(ComputedTrialResult(...))
        events = accel.process_events(computed, ctx)
```

**Вычисляемые метрики:**
- `series_value` - член ряда a_n
- `partial_sum` - частичная сумма S_n
- `partial_sum_deviation` - |S_n - lim|
- `accel_value` - значение ускорения A_n
- `accel_value_deviation` - |A_n - lim|
- `events` - список обнаруженных событий

#### 6.5 Детекция событий
**Файл:** `domain/event.py`

**Типы событий:**
- `slow_accel` - |A_n - lim| > |S_n - lim|
- `monotone` - монотонность
- `divergent` - расходимость
- `sign_changed` - смена знака
- `second_diff` - рост второй разности

**Обработка событий:**
- `log_action_capacity` - максимальное число раз, которое можно логировавть событий.
- `stop_action_limit` - остановить выполнение после N событий.

#### 6.6 Логика фильтрации
Если событие остановило выполнение:
1. Определяется сегмент расходимости
2. Применяются фильтры (savgol, kolzur)
3. Вычисляется среднее отфильтрованных значений
4. Результат сохраняется в `filtered_results`

**Результат:** `list[TrialResult]`

---

### Шаг 7. Параллельное/последовательное выполнение

**Файлы:** `infra/trials/parallel_runner.py`, `infra/trials/sequential_runner.py`

```python
for result_chunk in runner.run(combinations):
    results.extend(result_chunk)
```

**SequentialTrialRunner:**
- Выполняет комбинации последовательно
- Yield'ит результаты по мере готовности

**ParallelTrialRunner:**
- Создаёт `pool = multiprocessing.Pool(process_count)`
- Распределяет комбинации по процессам
- Yield'ит результаты по мере готовности

**Результат:** `Iterator[list[TrialResult]]`

---

### Шаг 8. Сериализация результатов

**Файл:** `infra/export/serializer.py`

```python
dicts = serializer.to_dict(results)
```

**Преобразование `TrialResult` -> `dict`:**
- Все поля flatten в плоскую структуру
- Числовые типы преобразуются в JSON-совместимые
- Сложные объекты сериализуются

---

### Шаг 9. Экспорт результатов

**Файлы:** `infra/export/*.py`

```python
for exporter in exporters:
    exporter.export(dicts, config, series)
```

#### JSON Exporter
**Файл:** `json_exporter.py`

```python
# Сохраняет в: output/results.json
json.dump(dicts, file, indent=2)
```

#### CSV Exporter
**Файл:** `csv_exporter.py`

```python
# Сохраняет в: output/results.csv
df = pd.DataFrame(dicts)
df.to_csv(path, index=False)
```

--

### Шаг 10. Визуализация (Frontend)

#### Web UI

1. **React UI** -> `GET /api/results`
2. **FastAPI** -> MongoDB с фильтрацией
3. **MongoDB** -> возвращает документы
4. **FastAPI** -> `ResultDocument` Pydantic модели
5. **React UI** -> отображает таблицы и графики

#### Vizr

```bash
PYTHONPATH="." python3 -m vizr.main ../consumer/output/results.json
```

1. Загружает отфильтрованные JSON файл через Polars
2. Строит графики сходимости через PyQtGraph

---

## Данные и их трансформация

### Входные данные

| Источник | Формат | Содержимое |
|----------|--------|------------|
| `options.json` | JSON | Конфигурация выполнения |
| `example.json` | JSON | Ряды, методы, шумы, фильтры |
| `example_series.csv` | CSV | Натуральные ряды |

### Промежуточные данные

| Стадия | Тип | Описание |
|--------|-----|----------|
| Параметры | `BaseSeriesParam`, `BaseAccelParam` | Загруженные параметры |
| Комбинации | `tuple[Series, Accel]` | Декартово произведение |
| Trial result | `TrialResult` | Результат одного trial |
| Сериализованные | `dict` | JSON-совместимые словари |

### Выходные данные

| Формат | Расположение | Использование |
|--------|--------------|---------------|
| JSON | `output/results.json` | Анализ, интеграция |
| CSV | `output/results.csv` | Excel, pandas |
| Parquet | `output/results.parquet` | Vizr, big data |
| MongoDB | База данных | Web UI |

---

## Схема данных TrialResult

```
TrialResult
├── SeriesTrialResult
│   ├── id: str
│   ├── name: str
│   ├── lim: float | None
│   └── arguments: dict
├── AccelTrialResult
│   ├── name: str
│   ├── m_value: int
│   └── additional_args: dict
├── computed: list[ComputedTrialResult]
│   ├── n: int
│   ├── series_value: float
│   ├── partial_sum: float
│   ├── partial_sum_deviation: float
│   ├── accel_value: float
│   ├── accel_value_deviation: float
│   └── events: list[Event]
├── noise: NoiseConfig | None
├── error: ErrorTrialResult | NoErrorTrialResult
└── filtered: FilteredResults | None
    ├── start_n: int
    ├── segment_length: int
    └── methods: dict[str, FilterMethodResult]
```
