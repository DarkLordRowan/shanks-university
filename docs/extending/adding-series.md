# Добавление новых рядов

## Обзор

Документ описывает пошаговую процедуру добавления нового математического ряда в фреймворк.

---

## Шаг 1. Создайте заголовочный файл

**Расположение:** `backend/core/include/series_iterators/`

**Имя файла:** `my_series_iterator.hpp` (snake_case)

```cpp
#ifndef MY_SERIES_ITERATOR_HPP
#define MY_SERIES_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

namespace shanks { namespace series {

template<AcceptedLike T, UnsignedIntLike K>
class my_series_iterator final : public series_base_succ<T, K> {
public:
    // Конструктор
    my_series_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid())
            throw std::invalid_argument("Invalid series argument");
    }

    // Теоретическое значение предела ряда
    T get_sum() const override;

    // Валидация параметра x
    bool is_invalid() const override;

    // Вычисление следующего члена ряда
    T next(K n, T& state) const override;
};

}} // namespace shanks

#endif
```

---

## Шаг 2. Реализуйте методы

### get_sum()

Возвращает теоретическое значение предела ряда.

```cpp
T get_sum() const override {
    return utils::exp(this->x);  // пример для exp(x)
}
```

### is_invalid()

Проверяет валидность параметра `x`.

```cpp
bool is_invalid() const override {
    return !utils::isfinite(this->x);
}
```

### next(K n, T& state)

Вычисляет (n+1)-й член ряда.

```cpp
T next(K n, T& state) const override {
    if (n == 0) {
        state = utils::cast<T>(1);
    } else {
        state *= this->x / utils::cast<T>(n);
    }
    return state;
}
```

**Параметры:**
- `n` - индекс (начинается с 0)
- `state` - текущее состояние (используется для рекурсивных вычислений)

**Возвращает:** значение (n+1)-го члена ряда

---

## Шаг 3. Зарегистрируйте ряд

**Файл:** `backend/core/include/series_registry.def`

Добавьте запись (до `SERIES_LAST`):

```cpp
SERIES_ENTRY(my_series_iterator, "MySeries")
```

**Именование:**
- Первый параметр: snake_case имя файла/класса без `_iterator`
- Второй параметр: CamelCase имя для Python/JSON

---

## Шаг 4. Включите заголовок

**Файл:** `backend/core/include/series_iterators/series_iterators.hpp`

Добавьте:

```cpp
#include "my_series_iterator.hpp"
```

---

## Шаг 5. Пересоберите pyshanks

```bash
cd backend/runner
./install_pyshanks.sh
```

Или вручную:

```bash
cd backend/bindings
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

---

## Шаг 6. Проверьте в Python

```python
import pyshanks as ps

# Класс доступен с суффиксом точности
cls = getattr(ps, "MySeriesF64")
instance = cls(1.0)
result = instance.generate(100)

print(result.Sn[-1])   # последняя частичная сумма
print(result.an[-1])   # последний член ряда
print(instance.get_sum())  # теоретическое значение предела
```

---

## Шаг 7. Используйте в JSON конфиге

**Файл:** `backend/runner/config/example.json`

```json
{
  "series": [
    {
      "name": "MySeries",
      "args": {"x": [1, 2, 3]}
    }
  ],
  "methods": [...]
}
```
