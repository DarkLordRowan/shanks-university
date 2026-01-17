# Shanks runner CLI

## Установка

Убедитесь что у вас установлен Python.

```bash
git clone https://github.com/DarkLordRowan/shanks-university.git
cd shanks-university
git checkout concept-proved
cd runner

bash install_pyshanks.sh

pip install -r requirements.txt
```

## Использование

### Базовое использование

```bash
python main.py run
```

Запускает со стандартными расположениями файлов:

* Ряды: `config/example.json` и `data/example_series.csv`

* Ускорение: `data/example.json`

* Вывод: `output/output.json`, `output/output.csv`, `output/events.json`, `output/events.csv`

* Графики: `plots/`

### Файл конфигурации испытания

```bash
python main.py run --options-json options.json
```

Запускает испытание с настройками в файле `options.json`. Подробнее о настройках ниже.

### Пользовательские входные файлы

```bash
python main.py run \
    --series-json data/custom_series.json \
    --series-csv data/custom_data.csv \
    --accel-json data/accel_config.json
```

### Выборочная обработка

```bash
# Пропустить детектирование событий
python main.py run --no-events

# Пропустить генерацию графиков
python main.py run --no-plots

# Пропустить и события и графики
python main.py run --no-events --no-plots
```

### Подробный вывод

```bash
# Базовый подробный вывод
python main.py run -v

# Детальный вывод с трейсбэком при ошибках
python main.py run -vv
```

## Аргументы

### Опции ввода

* `--options-json`: JSON файл для параметров испытания (вместо CLI флагов)

* `--series-json`: JSON файл для параметров серий (по умолчанию: `data/example.json`)

* `--series-csv`: CSV файл для натуральных рядов (по умолчанию: `data/example_series.csv`)

* `--accel-json`: JSON файл для параметров ускорения (по умолчанию: `data/example.json`)

### Опции вывода

* `--output-dir`: Директория для выходных файлов (по умолчанию: `output/`)

* `--plots-dir`: Директория для изображений графиков (по умолчанию: plots/)

* `--results-filename`: Базовое имя файлов результатов (без расширения, по умолчанию: results)

* `--results-json`: Пользовательский JSON файл для результатов

* `--results-csv`: Пользовательский CSV файл для результатов

* `--events-json`: Пользовательский JSON файл для событий

* `--events-csv`: Пользовательский CSV файл для событий

### Флаги обработки

* `--no-events`: Пропустить детектирование и экспорт событий

* `--no-plots`: Пропустить генерацию графиков

* `--output-formats`: Выбрать форматы вывода (json, csv, parquet) - по умолчанию: json csv

* `--with-arb`: Включить ARB режим (пока не поддерживается)

* `--verbose/-v`: Увеличить уровень детализации (используйте `-v` для базового, `-vv` для детального)

### Флаги выполнения

* `--trial-process-count`: количество создаваемых процессов для обработки испытания (по умолчанию `1`)

* `--trial-task-timeout`: максимальное время для ожидания выполнения одной задачи в секундах (по умолчанию `10`)

### Подробнее про форматы вывода

С помощью флага `--output-formats` можно выбрать один или несколько форматов вывода:

* `json` - классический формат JSON (по умолчанию)
* `csv` - табличный формат CSV (по умолчанию) 
* `parquet` - высокоэффективный формат для больших данных

Примеры использования:
```bash
# Только JSON с именем по умолчанию (results.json)
python main.py run --output-formats json

# CSV и Parquet с кастомным именем
python main.py run --output-formats csv parquet --results-filename my_experiment

# Все три формата с именем experiment_2024
python main.py run --output-formats json csv parquet --results-filename experiment_2024
```

Результат:
```
output/
├── results.json
├── results.csv
└── results.parquet
```

Parquet файлы сохраняются в той же директории `output/` с тем же именем, что и другие форматы, но с расширением `.parquet`. 

### Оптимизация Parquet:
- **Сжатие ZSTD**: Максимальный уровень сжатия (уровень 9) для минимального размера файлов
- **Оптимизированная схема**: Явно определенная схема данных для лучшей производительности
- **Статистика**: Включена статистика для ускорения запросов
- **Размер групп строк**: 10,000 записей для баланса между сжатием и чтением

Этот формат обеспечивает быстрое чтение и запись, особенно для больших объемов данных.

### Ряды в формате CSV

Утилита поддерживает заданные ряды вручную в формате `.csv`
Укажите доступ к ним по флагу `--series-csv` или в конфигурации по пути `--options-json`. Внутри файла структура рядов следующая:

```csv
1,2,3,4,5,6,7,8,9
9,8,7,6,5,4,3,2,1
```

В этом примере расположены два ряда, члены которых перечислены в строчке через запятую; заголовки и отступы не должны присутствовать.

## Выходные файлы

**Результаты** (`results`): Содержит результаты выполнения испытаний в форматах JSON и CSV

**События** (`events`): Содержит обнаруженные события из анализа испытаний

**Графики** (`plots`): Визуальные представления результатов анализа

## Примеры

### Быстрый анализ с пользовательскими данными

```bash
python main.py --series-json my_data.json --output-dir quick_results -v
```

### Продуктивный запуск со всеми выводами

```bash
python main.py run \
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
python main.py --no-events --no-plots --output-dir test_run
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
      "name": "ShanksTransformAlternating",
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

## Разработка

### 0. Установите проект:

```sh
git clone https://github.com/DarkLordRowan/shanks-university.git
cd shanks-university
git checkout concept-proved
cd runner

# Убедитесь, что у вас установлены зависимости:
sudo apt update
sudo apt install python3 python3-pip cmake build-essential
```

Для работы нужно собрать библиотеку **pyshanks*
```
bash install_pyshanks.sh
```

### 1. Установите [Poetry](https://python-poetry.org/docs/)

```sh
# Linux/macOS/WSL
curl -sSL https://install.python-poetry.org | python3 -

# Windows (PowerShell)
(Invoke-WebRequest -Uri https://install.python-poetry.org -UseBasicParsing).Content | python -

poetry --version
```

### 2. Загрузите зависимости:

```sh
poetry install
```

### 3. Установите pre-commit хуки (сделайте это ОДИН РАЗ после poetry install):

```sh
poetry run pre-commit install
```

### 4. Запуск скрипта без входа в окружение:

```sh
poetry run python main.py run
```
...



## Запуск в Docker Compose

Сборка в изолированном окружении с автосборкой

### 0. Зависимости

В отличии от остальных способов, из прямых зависимостей только [Docker](https://www.docker.com/get-started/) и [Docker Compose](https://docs.docker.com/compose/install/).

### 1. Настройка

```sh
cp .env.example .env
```

### 2. Запуск

```sh
docker compose up -d --build
# Отслеживайте статус запуска контейнеров
docker compose ps
```

### 3. Выполнение команд

Для выполнения команд используйте шаблон:

```sh
docker compose exec -it shanks-runner аналогичная_python_команда
# Например:
docker compose exec -it shanks-runner python main.py run --options-json config/options.json
```

### 4. Ограничения

Ожидается, что конфигурация будет в директории `config`, а вывод в `output`. Можно составлять поддиректории, но контейнер не имеет доступ к внешним директориям.

WatchFiles *пока не поддерживается*, после изменений в исходном коде нужно пересобирать образы(не касается конфигураций!).

Для скорости сборки образ опирается на `requirements.txt`, при добавлении/исключении Python зависимостей через Poetry обновляйте и `requirements.txt`:

```sh
poetry export --without-hashes --format=requirements.txt > requirements.txt
```
