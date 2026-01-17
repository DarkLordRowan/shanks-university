# Расширение системы - обзор

## Концепция

Фреймворк поделён на независимые модули.

- Каждый компонент (ряд, метод, фильтр) является независимым модулем.
- Компоненты регистрируются в реестрах через макросы.
- Все компоненты автоматически становятся доступными в Python через pybind11.
- Новые компоненты добавляются без изменения существующего кода.

```
┌─────────────────────────────────────────────────────────────┐
│                     C++ Computational Core                  │
│  ┌──────────────┐    ┌──────────────────┐    ┌───────────┐  │
│  │   New Series │--->│ Series Registry  │--->│ pyshanks  │  │
│  │   ( ваш код )│    │  (macro-based)   │    │ (auto)    │  │
│  └──────────────┘    └──────────────────┘    └───────────┘  │
│                                                             │
│  ┌──────────────┐    ┌──────────────────┐    ┌───────────┐  │
│  │  New Method  │───>│ Method Registry  │───>│ pyshanks  │  │
│  │   ( ваш код )│    │  (macro-based)   │    │ (auto)    │  │
│  └──────────────┘    └──────────────────┘    └───────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              v
                        ┌───────────┐
                        │   JSON    │
                        │  Config   │
                        └───────────┘
```

## Точки расширения

### 1. Новые ряды (Series)

**Что:** Математические ряды для исследования.

**Где:** `backend/core/include/series_iterators/`

**Базовый класс:** `series_base_succ<T, K>` (альтернативно: `series_base_iter<T, K, T>` или базовый `series_base<T, K>`)

**Регистрация:** `series_registry.def`

**Использование:** `example.json` -> `series` секция

---

### 2. Новые методы ускорения (Methods)

**Что:** Алгоритмы ускорения сходимости рядов.

**Где:** `backend/core/include/transformations/`

**Базовый класс:** `series_acceleration<T, K>`

**Регистрация:** `transformation_registry.def`

**Использование:** `example.json` -> `methods` секция

---

### 3. Новые события (Events)

**Что:** Критерии останова и детекции аномалий.

**Где:** `backend/runner/` (модуль `domain/event.py`)

**Регистрация:** `EventType` enum

**Использование:** `example.json` -> `methods[*].events`

---

## Механизм реестров

### Series Registry

**Файл:** `backend/core/include/series_registry.def`

**Макросы:**
```cpp
SERIES_ENTRY(snake_name, "CamelName")         // Обычный ряд
SERIES_ENTRY_ARGS(name, "Name", "arg", "arg") // Ряд с аргументом
SERIES_LAST(snake_name, "CamelName")          // Последний ряд
```

**Пример:**
```cpp
SERIES_ENTRY(cos, "CosSeries")
SERIES_ENTRY_ARGS(bin, "BinSeries", "alpha", "alpha")
SERIES_LAST(x, "XSeries")
```

**Генерация:**
- Класс `CosSeriesF64` доступен в Python
- В JSON конфиге: `{"name": "CosSeries", ...}`

---

### Transformation Registry

**Файл:** `backend/core/include/transformation_registry.def`

**Макрос:**
```cpp
TRANSFORMATION_ENTRY(id, "Name", { return std::make_unique<ClassName<T, K>>(...args...); })
```

**Пример:**
```cpp
TRANSFORMATION_ENTRY(shanks_transformation_id, "Shanks Transformation", {
    return std::make_unique<shanks::algos::shanks_algorithm<T, K>>();
})
```

**Генерация:**
- Класс `ShanksAlgorithmF64` доступен в Python
- В JSON конфиге: `{"name": "ShanksAlgorithm", ...}`

---

## Цикл разработки

```
1. Написать C++ класс
       │
       v
2. Зарегистрировать в реестре
       │
       v
3. Включить заголовок в methods.hpp / series.hpp
       │
       v
4. Пересобрать pyshanks
       │
       v
5. Использовать в JSON конфиге или напрямую через Python-скрипт, использующий `pyshanks`.
```

---

## Классы-основы

### Ряд (Series)

**Файл:** `series_base.hpp`

```cpp
template <AcceptedLike T, UnsignedIntLike K>
class series_base_succ : public series_base_iter<T, K, T> {
public:
    series_base_succ(T x) : series_base_iter<T, K, T>(x) {}

    // Теоретическое значение предела ряда
    virtual T get_sum() const override = 0;

    // Начальное состояние итератора
    T initial_state() const { return utils::cast<T>(0); };

    // Вычислить следующий член ряда
    virtual T next(K index, T& state) const = 0;
};
```

**Методы:**
- `get_sum()` - предел ряда (теоретическое значение)
- `next(index, state)` - вычислить i-й член ряда

---

### Метод ускорения (Acceleration)

**Файл:** `transformations_info.hpp`

```cpp
template <typename T, typename K>
concept series_acceleration = requires(T t, series_iter<T, K> s, K n, K m) {
    { t(s, n, m) } -> std::same_as<series_iter<T, K>>;
};
```

**Требования:**
- Вызываемый объект (функтор или класс с operator())
- Принимает: `series_iter`, `n`, `m`
- Возвращает: `series_iter`

---

## Шаблонные параметры

### T - тип данных

| Значение | Описание |
|----------|----------|
| `float` | 32-бит float |
| `double` | 64-бит double |
| `long double` | Extended precision |
| `mpfr::mpreal` | Arbitrary precision |

### K - тип индекса

| Значение | Описание |
|----------|----------|
| `std::size_t` | Беззнаковое целое (по умолчанию) |

---

## Пересборка pyshanks

```bash
cd backend/bindings
./install_pyshanks.sh
```

Или вручную:

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

## Проверка

После добавления компонента:

1. **Python:**
```python
import pyshanks as ps
# Ряд
cls = getattr(ps, "MySeriesF64")
instance = cls(1.0)
result = instance.generate(100)
# Метод
cls = getattr(ps, "MyAlgorithmF64")
instance = cls()
```

2. **JSON конфиг:**
```json
{
  "series": [{"name": "MySeries", "args": {"x": 1}}],
  "methods": [{"name": "MyAlgorithm", "n": [10], "m": [2]}]
}
```

---

## Следующие документы

- [Добавление новых рядов](adding-series.md) - пошаговая инструкция
- [Добавление методов ускорения](adding-methods.md) - пошаговая инструкция
