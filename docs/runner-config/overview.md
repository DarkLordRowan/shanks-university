# Система конфигурации - обзор

## Двухуровневая архитектура

Конфигурация Python Runner разделена на два уровня:

```
┌─────────────────────────────────────────────────────────────┐
│                    УРОВЕНЬ 1: Выполнение                    │
│                     options.json / options.yaml             │
│  - Как запускать эксперименты                               │
│  - Параллелизация, таймауты, формат вывода                  │
│  - Пути к файлам конфигурации экспериментов                 │
└─────────────────────────────────────────────────────────────┘
                              │
                              v
┌─────────────────────────────────────────────────────────────┐
│                УРОВЕНЬ 2: Эксперименты                      │
│              example.json / big_boy.json                    │
│  - Что запускать (ряды, методы)                             │
│  - Параметры алгоритмов                                     │
│  - Шумы и фильтры                                           │
│  - События и критерии останова                              │
└─────────────────────────────────────────────────────────────┘
```

## Уровень 1: TrialConfig (options.json/yaml)

**Расположение:** `backend/runner/config/options.json` или `options.yaml`
(Конкретный путь задаётся с помощью флага `--config`)

**Назначение:** Конфигурация выполнения экспериментов (how to run).

**Категории параметров:**

### 1. Пути к файлам

| Параметр | Описание | По умолчанию |
|----------|----------|--------------|
| `series_json` | Путь к JSON с рядами | `config/example.json` |
| `accel_json` | Путь к JSON с методами ускорения | `config/example.json` |
| `noise_json` | Путь к JSON с шумами | `config/example.json` |
| `filters_json` | Путь к JSON с фильтрами | `config/example.json` |

**Важно:** Все пути могут указывать на один и тот же файл. Например, `example.json` содержит все четыре секции: `series`, `methods`, `noises`, `filters`.

### 2. Вывод результатов

| Параметр | Описание | По умолчанию |
|----------|----------|--------------|
| `output_dir` | Директория для выходных файлов | `output` |
| `results_filename` | Базовое имя файла результатов | `results` |
| `output_formats` | Форматы экспорта | `[JSON, CSV]` |

### 3. Выполнение

| Параметр | Описание | По умолчанию |
|----------|----------|--------------|
| `trial_process_count` | Число параллельных процессов | 1 (последовательно) |
| `trial_task_timeout` | Таймаут одного trial (сек) | 10 |
| `trial_memory_efficient` | Режим экономии памяти | true |
| `verbose` | Уровень логирования (0-2) | 0 |
| `no_events` | Отключить детекцию событий | false |

**Режимы выполнения:**
- `trial_process_count = 1` → `SequentialTrialRunner`
- `trial_process_count > 1` → `ParallelTrialRunner`

### 4. Точность

| Параметр | Описание | Возможные значения |
|----------|----------|-------------------|
| `precisions` | Типы точности вычислений | `F32`, `F64`, `FLong`, `Arb`, `CF32`, `CF64`, `CFLong`, `CArb` |

**Пример:**
```yaml
precisions:
  - F64
  - Arb
```

Эксперименты будут выполнены дважды - один раз для каждой точности.

---

## Уровень 2: Конфигурация экспериментов

**Расположение:** `backend/runner/config/example.json`, `big_boy.json`

**Назначение:** Описание того, что запускать (what to run).

Структура этого файла объяснена в: [Experiment Config](experiment-config.md).

### Структура файла

```json
{
  "series": [...],   // Ряды для исследования
  "noises": [...],   // Шумы для добавления
  "filters": [...],  // Фильтры для обработки
  "methods": [...]   // Методы ускорения
}
```

### Связь с TrialConfig

```yaml
# options.yaml
series_json: config/example.json    # -> использует series секцию
accel_json: config/example.json     # -> использует methods секцию
noise_json: config/example.json     # -> использует noises секцию
filters_json: config/example.json   # -> использует filters секцию
```

**Ключевое свойство:** Один файл может содержать все секции. TrialConfig определяет, какие секции из какого файла брать.

**Пример разделения:**
```yaml
# Использовать разные файлы для разных секций
series_json: config/my_series.json
accel_json: config/my_methods.json
noise_json: config/my_noises.json
filters_json: config/my_filters.json
```

## Пример полного конфига

### options.yaml

```yaml
verbose: 1
series_json: config/example.json
accel_json: config/example.json
noise_json: config/example.json
filters_json: config/example.json
output_dir: output
trial_process_count: 4
precisions:
  - F64
output_formats:
  - JSON
  - CSV
```

### example.json

```json
{
  "series": [
    {"name": "CosSeries", "args": {"x": [1, 2]}}
  ],
  "noises": [
    {"type": "Normal", "method": "jitter", "param1": 0.0, "param2": 1e-10, "seed": 42}
  ],
  "filters": [
    {"type": "savitzky_golay", "params": {"polyorder": 2}}
  ],
  "methods": [
    {
      "name": "LevinAlgorithm",
      "n": {"start": 10, "stop": 50, "step": 10},
      "m": [8],
      "args": {"remainder": ["v_type"]},
      "events": [...]
    }
  ]
}
```

---

## Следующие документы

- [Experiment Config - конфигурация экспериментов](experiment-config.md)
