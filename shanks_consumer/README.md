# Shanks consumer CLI

## Использование

### Базовое использование

```bash
python main.py
```

Запускает со стандартными расположениями файлов:

* Ряды: `data/example.json` и `data/example_series.csv`

* Ускорение: `data/example.json`

* Вывод: `output/output.json`, `output/output.csv`, `output/events.json`, `output/events.csv`

* Графики: `plots/`

### Пользовательские входные файлы

```bash
python main.py \
    --series-json data/custom_series.json \
    --series-csv data/custom_data.csv \
    --accel-json data/accel_config.json
```

### Пользовательские выходные директории

```bash
python script.py \
    --output-dir results/my_analysis \
    --plots-dir figures/my_analysis \
    --results-json results/my_analysis/final_results.json \
    --events-csv results/my_analysis/detected_events.csv
```

### Выборочная обработка

```bash
# Пропустить детектирование событий
python script.py --no-events

# Пропустить генерацию графиков
python script.py --no-plots

# Пропустить и события и графики
python script.py --no-events --no-plots
```

### Подробный вывод

```bash
# Базовый подробный вывод
python script.py -v

# Детальный вывод с трейсбэком при ошибках
python script.py -vv
```

## Аргументы

### Опции ввода

* `--series-json`: JSON файл для параметров серий (по умолчанию: `data/example.json`)

* `--series-csv`: CSV файл для параметров серий (по умолчанию: `data/example_series.csv`)

* `--accel-json`: JSON файл для параметров ускорения (по умолчанию: `data/example.json`)

### Опции вывода

* `--output-dir`: Директория для выходных файлов (по умолчанию: `output/`)

* `--plots-dir`: Директория для изображений графиков (по умолчанию: plots/)

* `--results-json`: Пользовательский JSON файл для результатов

* `--results-csv`: Пользовательский CSV файл для результатов

* `--events-json`: Пользовательский JSON файл для событий

* `--events-csv`: Пользовательский CSV файл для событий

### Флаги обработки

* `--no-events`: Пропустить детектирование и экспорт событий

* `--no-plots`: Пропустить генерацию графиков

* `--verbose/-v`: Увеличить уровень детализации (используйте `-v` для базового, `-vv` для детального)

## Выходные файлы

**Результаты** (`results`): Содержит результаты выполнения испытаний в форматах JSON и CSV

**События** (`events`): Содержит обнаруженные события из анализа испытаний

**Графики** (`plots`): Визуальные представления результатов анализа

## Примеры

### Быстрый анализ с пользовательскими данными

```bash
python script.py --series-json my_data.json --output-dir quick_results -v
```

### Продуктивный запуск со всеми выводами

```bash
python script.py \
    --series-json production/series_config.json \
    --series-csv production/series_data.csv \
    --accel-json production/accel_config.json \
    --output-dir production/results \
    --plots-dir production/figures \
    --results-json production/results/final_analysis.json \
    -vv
```

### Минимальный вывод для тестирования

```bash
python script.py --no-events --no-plots --output-dir test_run
```

## Конфигурация испытания

Типовой `example.json`

```json
{
  // Список рядов
  "series": [ 
    {
      // Точное название ряда
      "name": "ExpSeries",
      // Словарь аргументов ряда
      "args": {
        // Можно присвоить значение списка, тогда для каждого из параметров 
        // будет создан соответствующий ряд
        "x": [1, 2, 3, 4]
      }
    },
    {
      "name": "CosSeries",
      "args": {
        "x": {
          // С помощью этого синтаксиса можно задать список параметров ряда
          // срезом [start, stop, step]
          "start": 1,
          "stop": 11,
          "step": 1
        }
      }
    }
  ],
  // Список методов
  "methods": [
    {
      // Точное название метода
      "name": "ShanksAlgorithm",
      // Базовые атрибуты метода задаются аналогично
      "n": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
      // Можно выставить всего одно значение без списка
      "m": 4,
      // Дополнительные аргументы метода, у этого метода их нет
      "args": {}
    },
    {
      "name": "ShanksTransformAlternatingAlgorithm",
      "n": {
        // Рендж можно применить и для этих аргументов
        "start": 1,
        "stop": 11,
        "step": 1
      },
      "m": {
        "start": 2,
        "stop": 10,
        "step": 3
      },
      "args": {}
    },
    {
      "name": "LevinAlgorithm",
      "n": {
        "start": 1,
        "stop": 11,
        "step": 1
      },
      "m": [8],
      "args": {
        // Дополнительные аргументы задаются добровольно - у них есть значения по умолчанию
        // Можно использовать списки для вариации параметров
        "remainder": ["v_variant", "u_variant", "t_wave_variant"],
        "useRecFormulas": true,
        // И ренджи, если параметр представлен числом
        "beta": {
            "start": 0,
            "stop": 1,
            "step": 0.1
        }
      }
    }
  ]
}

```
