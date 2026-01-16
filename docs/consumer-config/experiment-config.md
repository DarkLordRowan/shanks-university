# Инструкции экспериментов

**Расположение:** `backend/runner/config/`

## Назначение

Файлы инструкций `example.json`, `big_boy.json` и подобные содержат описание **что** запускать - ряды, методы ускорения, шумы, фильтры и события.


## Структура файла инструкций

```json
{
  "series": [...],
  "noises": [...],
  "filters": [...],
  "methods": [...]
}
```

---

## Секция `series`

Определяет математические ряды для исследования.

### Структура

```json
{
  "series": [
    {
      "name": "SeriesName",
      "args": {
        "param1": [value1, value2],
        "param2": {"start": 1, "stop": 10, "step": 1}
      }
    }
  ]
}
```

### Поля

| Поле | Тип | Обязательно | Описание |
|------|-----|-------------|----------|
| `name` | `string` | Да | Имя ряда (зарегистрированного в C++ registry) |
| `args` | `object` | Нет | Аргументы конструктора ряда |
| `vecSize` | `int` | Нет | Размер генерируемой последовательности |

### Примеры

#### Ряд с одним значением параметра

```json
{
  "name": "CosSeries",
  "args": {"x": 1}
}
```

Генерируется 1 ряд.

#### Ряд с несколькими значениями

```json
{
  "name": "CosSeries",
  "args": {"x": [1, 2, 3]}
}
```

Генерируется 3 ряда (для x=1, x=2, x=3).

#### Ряд с диапазоном

```json
{
  "name": "ExpSeries",
  "args": {"x": {"start": 0, "stop": 1, "step": 0.1}}
}
```

Генерируется 11 рядов (x = 0, 0.1, 0.2, ..., 1).

#### Комбинации параметров

```json
{
  "name": "LogSeries",
  "args": {
    "x": [1, 2, 3],
    "base": [2, 10]
  }
}
```

Генерируется 3 × 2 = 6 рядов.

---

## Секция `noises`

Определяет шум, добавляемый к рядам.

### Структура

```json
{
  "noises": [
    {
      "type": "Normal|Uniform",
      "method": "jitter|scaling",
      "param1": float,
      "param2": float,
      "seed": int
    }
  ]
}
```

### Поля

| Поле | Тип | По умолчанию | Описание |
|------|-----|--------------|----------|
| `type` | `string` | `Uniform` | Тип распределения шума |
| `method` | `string` | `jitter` | Метод применения шума |
| `param1` | `float` | `0.0` | Параметр 1 (зависит от type/method) |
| `param2` | `float` | `0.0` | Параметр 2 (зависит от type/method) |
| `seed` | `int` | `0` | Seed для генератора случайных чисел |

### Типы шумов

| `type` | `method` | `param1` | `param2` | Описание |
|--------|----------|---------|---------|----------|
| `Normal` | `jitter` | mean | std | Нормальный шум (гауссовский дрожание) |
| `Uniform` | `scaling` | min | max | Равномерный мультипликативный шум |

### Примеры

#### Нормальный шум

```json
{
  "type": "Normal",
  "method": "jitter",
  "param1": 0.0,
  "param2": 1e-10,
  "seed": 42
}
```

Добавляет гауссовский шум с mean=0, std=1e-10.

#### Равномерный шум

```json
{
  "type": "Uniform",
  "method": "scaling",
  "param1": 0.9999999999,
  "param2": 1.0000000001,
  "seed": 42
}
```

Масштабирует значения случайным коэффициентом из [0.9999999999, 1.0000000001].

---

## Секция `filters`

Определяет фильтры для обработки результатов при обнаружении событий остановки (например, расходимости).

### Структура

```json
{
  "filters": [
    {
      "type": "savgol|kolzur",
      "params": {...}
    }
  ]
}
```

### Поля

| Поле | Тип | Обязательно | Описание |
|------|-----|-------------|----------|
| `type` | `string` | Да | Тип фильтра |
| `params` | `object` | Нет | Параметры фильтра |

### Типы фильтров

#### Savitzky-Golay (`savgol`)

```json
{
  "type": "savgol",
  "params": {
    "polyorder": 2,
    "derive": 0,
    "delta": 1.0,
    "window_length": "segment"  // или число
  }
}
```

**Параметры:**
- `polyorder` - порядок полинома (по умолчанию 2)
- `derive` - порядок производной (по умолчанию 0)
- `delta` - шаг дискретизации (по умолчанию 1.0)
- `window_length` - длина окна (`"segment"` для автоматической или число)

#### Kolzur (`kolzur`)

```json
{
  "type": "kolzur",
  "params": {
    "degree": 3
  }
}
```

**Параметры:**
- `degree` - степень фильтра

### Примеры

```json
{
  "filters": [
    {
      "type": "savgol",
      "params": {
        "polyorder": 2,
        "derive": 0,
        "delta": 1.0
      }
    },
    {
      "type": "kolzur",
      "params": {
        "degree": 3
      }
    }
  ]
}
```

---

## Секция `methods`

Определяет методы ускорения сходимости и их параметры.

### Структура

```json
{
  "methods": [
    {
      "name": "MethodName",
      "n": {...} | [...],
      "m": [...],
      "args": {...},
      "events": [...]
    }
  ]
}
```

### Поля

| Поле | Тип | Обязательно | Описание |
|------|-----|-------------|----------|
| `name` | `string` | Да | Имя метода (зарегистрированного в C++ registry) |
| `n` | `range`/`array` | Да | Значения параметра n |
| `m` | `array` | Да | Значения параметра m |
| `args` | `object` | Нет | Дополнительные аргументы метода |
| `events` | `array` | Нет | Спецификации событий |

### Параметр `n`

Задаёт значения n (количество членов ряда для ускорения).

#### Диапазон

```json
"n": {"start": 10, "stop": 100, "step": 10}
```

Генерирует `[10, 20, 30, ..., 100]`.

#### Массив

```json
"n": [5, 10, 20, 50]
```

Использует указанные значения.

### Параметр `m`

Задаёт значения m (параметр ускорения).

```json
"m": [2, 4, 8]
```

### Параметр `args`

Дополнительные аргументы конструктора метода. Поддерживает генерацию комбинаций.

#### Пример для LevinAlgorithm

```json
"args": {
  "remainder": ["v_type", "u_type"],
  "useRecurrentFormula": [true, false],
  "beta": [0.1, 1, 10]
}
```

Генерирует 2 × 2 × 3 = 12 комбинаций.

#### Общие параметры

| Метод | Аргумент | Возможные значения |
|-------|----------|-------------------|
| LevinAlgorithm | `remainder` | `v_type`, `u_type`, `t_wave_type` |
| LevinAlgorithm | `useRecurrentFormula` | `true`, `false` |
| LevinAlgorithm | `beta` | числа |
| LevinSidiMAlgorithm | `gamma` | числа |
| LevinSidiSAlgorithm | `parameter` | числа |
| WynnRhoAlgorithm | `numerator` | `rho_type`, `generalized_type`, `gamma_rho_type` |
| WynnRhoAlgorithm | `gamma` | числа |
| WynnRhoAlgorithm | `RHO` | числа |
| WynnEpsilon3Algorithm | `epsilon_threshold` | числа |

### Параметр `events`

Определяет события для детекции и реакцию на них.

```json
"events": [
  {
    "type": "slow_accel",
    "log_action_capacity": 2
  },
  {
    "type": "divergent",
    "stop_action_limit": 3
  }
]
```

#### Поля события

| Поле | Тип | Описание |
|------|-----|----------|
| `type` | `EventType` | Тип события |
| `log_action_capacity` | `int \| null` | Лимит логирования (null = неограниченно) |
| `stop_action_limit` | `int \| null` | Лимит перед остановкой (null = не останавливать) |

#### Типы событий

| `type` | Описание |
|--------|----------|
| `slow_accel` | Метод медленнее частичных сумм |
| `monotone` | Монотонное поведение |
| `divergent` | Расходимость |
| `sign_changed` | Смена знака |
| `second_diff` | Рост второй разности |

#### Логика событий

- **`log_action_capacity`** - после N событий этого типа система перестаёт их логировать.
- **`stop_action_limit`** - после N событий этого типа система останавливает выполнение trial.

При остановке по `stop_action_limit`:
1. Определяется сегмент расходимости.
2. Применяются фильтры из секции `filters`, если остановка вызвана событиями.
3. Результат сохраняется в `filtered`.

---

## Полный пример

```json
{
  "series": [
    {
      "name": "CosSeries",
      "args": {"x": [1, 2]}
    }
  ],
  "noises": [
    {
      "type": "Normal",
      "method": "jitter",
      "param1": 0.0,
      "param2": 1e-10,
      "seed": 42
    }
  ],
  "filters": [
    {
      "type": "savgol",
      "params": {
        "polyorder": 2,
        "derive": 0,
        "delta": 1.0
      }
    }
  ],
  "methods": [
    {
      "name": "LevinAlgorithm",
      "n": {"start": 10, "stop": 50, "step": 10},
      "m": [8],
      "args": {
        "remainder": ["v_type"],
        "useRecurrentFormula": true
      },
      "events": [
        {
          "type": "slow_accel",
          "log_action_capacity": 2
        },
        {
          "type": "divergent",
          "stop_action_limit": 3
        }
      ]
    }
  ]
}
```

---

## Вычисление количества trial'ов

```
total_trials = (
    len(series_combinations) *
    len(method_combinations) *
    len(n_values) *
    len(m_values) *
    len(noises) *
    len(precisions)
)
```

**Пример:**
- `series`: 2 значения x => 2 ряда
- `methods`: 1 метод с 1 комбинацией args
- `n`: 5 значений (10, 20, 30, 40, 50)
- `m`: 1 значение (8)
- `noises`: 1 шум
- `precisions`: 1 (F64)

```
total = 2 × 1 × 5 × 1 × 1 × 1 = 10 trial'ов
```

С комбинациями args:

```
series: x=[1,2] -> 2
methods: remainder=[v_type, u_type], useRecurrentFormula=[true, false] -> 4
n: 5
m: 1
noises: 1
precisions: 1

total = 2 × 4 × 5 × 1 × 1 × 1 = 40 trial'ов
```
