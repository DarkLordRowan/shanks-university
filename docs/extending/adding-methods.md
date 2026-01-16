# Добавление методов ускорения

## Обзор

Документ описывает пошаговую процедуру добавления нового алгоритма ускорения сходимости рядов в фреймворк.

---

## Шаг 1. Создайте заголовочный файл

**Расположение:** `backend/core/include/transformations/`

**Имя файла:** `my_algorithm.hpp` (snake_case)

```cpp
#ifndef MY_ALGORITHM_HPP
#define MY_ALGORITHM_HPP
#pragma once

#include "series_acceleration.hpp"

namespace shanks { namespace algos {

template <AcceptedLike T, UnsignedIntLike K>
class my_algorithm final : public series_acceleration<T, K> {
public:
    explicit my_algorithm(...args...);

    T operator()(
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;
};

}} // namespace shanks

#endif
```

---

## Шаг 2. Наследуйтесь от series_acceleration

**Базовый класс:** `series_acceleration<T, K>`

**Интерфейс:**

```cpp
template<AcceptedLike T, UnsignedIntLike K>
class series_acceleration
{
public:
    explicit series_acceleration(std::string name = "unknown");

    // Главный метод: вычислить ускоренную сумму
    virtual T operator()(
        const K n,           // Количество членов
        const K order,       // Порядок преобразования (m)
        const series_result<T>& data  // Данные ряда
    ) const = 0;

    virtual std::string get_name();
protected:
    std::string acceleration_name;
};
```

---

## Шаг 3. Реализуйте operator()

### Параметры

| Параметр | Тип | Описание |
|----------|-----|----------|
| `n` | `K` | Количество членов ряда для использования |
| `order` | `K` | Порядок преобразования (параметр m) |
| `data` | `series_result<T>` | Структура с данными ряда |

### series_result структура

```cpp
struct series_result<T> {
    std::vector<T> Sn;  // Частичные суммы: Sn[i] = S_{i+1}
    std::vector<T> an;  // Члены ряда: an[i] = a_{i+1}
};
```

**Важно:** Индексация в векторах начинается с 0:
- `Sn[0]` = S₁ (сумма первого члена)
- `Sn[n-1]` = S_n (сумма n членов)

### Возвращаемое значение

Ускоренное значение суммы ряда (тип `T`).

---

## Шаг 4. Проверьте размер данных

```cpp
// Минимально требуемый размер зависит от алгоритма
const K required_size = /* вычислить */;

if (data.Sn.size() < required_size) {
    throw std::out_of_range("Insufficient data size");
}
```

---

## Шаг 5. Зарегистрируйте алгоритм

**Файл:** `backend/core/include/transformation_registry.def`

```cpp
TRANSFORMATION_ENTRY(my_algorithm_id, "MyAlgorithm", {
    return std::make_unique<my_algorithm<T, K>>(...args...);
})
```

**Параметры макроса:**
1. Уникальный ID (snake_case с `_id` суффиксом)
2. Отображаемое имя (CamelCase)
3. Factory функция (лямбда, создающая экземпляр)

---

## Шаг 6. Включите заголовок

**Файл:** `backend/core/include/methods.hpp`

Добавьте:

```c++
#include "transformations/my_algorithm.hpp"
```

---

## Шаг 7. Пересоберите pyshanks

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

## Шаг 8. Проверьте в Python

```python
import pyshanks as ps

# Класс доступен с суффиксом точности
cls = getattr(ps, "MyAlgorithmF64")
instance = cls(...args...)

# Создаём данные ряда
series_data = ps.series_resultF64()
series_data.Sn = [1.0, 0.5, 0.75, ...]
series_data.an = [1.0, -0.5, 0.25, ...]

# Применяем алгоритм
result = instance(10, 2, series_data)
```
