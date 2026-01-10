# Добавление нового ряда

1. Реализация "итератора", вычисляющего a_{n} для нового ряда, должна быть реализована в /include/series_iterators (класс должен наследоваться от series_base_succ).

2. Добавление информации о новом ряде в файл series_registry.def
    - Название ряда для c++ кода (snake case)
    - Название ряда для python   (camel case)
Пример:
Для ряда exp(x) = \sum_{n=0}^{\infty}\frac{x^n}{n!}:
    - Название ряда для c++ кода -> exp
    - Название ряда для python   -> "ExpSeries"
По итогу в series_registry.def должна появиться строка:
    SERIES_ENTRY(exp, "ExpSeries")
Для удобства новые вхождение лучше вставлять сохраняя алфавитный порядок.

3. Добавить #include в /include/series_iterators/series_iterators.hpp

# Добавление нового метода

1. Реализация класса метода в include/transformations (класс должен наследоваться от series_acceleration)

2. Добавление информации о методе в файл transformation_registry.def
    - Наименование для реализованного метода для class enum transformation_id_t::
    - Название метода
    - Тело Лямбда функции (генератора) возвращающей unique_ptr реализованного метода
Пример: 
Для метода shanks_algorithm вхождение выглядит так:
    - Наименование для  transformation_id_t:: -> shanks_transformation_id
    - Название метода                         -> "Shanks Transformation"
    - Тело Лямбда фукнции (генератора)        -> { return std::make_unique<shanks::algos::shanks_algorithm<T, K>>(); }
По итогу в transformation_registry.def должна появиться строчка:
    TRANSFORMATION_ENTRY(shanks_transformation_id, "Shanks Transformation", { return std::make_unique<shanks::algos::shanks_algorithm<T, K>>(); })
Для удобство новые вхождение лучше вставлять, не нарушая алфавитный порядок.

В случае, если метод имеет разновидности, можно их уточнить.
Пример:
L-трансформация может быть вычислено через формулу напрямую или рекуррентно, в зависимости от параметров конструктора, тогда в transformation_registry.def можно добавить:
Для реализации формульной:
TRANSFORMATION_ENTRY(levin_sidi_l_transformation_id, "Levin Sidi-L Transformation", { return std::make_unique<shanks::algos::levin_algorithm<T, K>>(false); })
Для реализации рекуррентной:
TRANSFORMATION_ENTRY(recurrent_levin_sidi_l_transformation_id, "Levin Sidi-L Transformation", { return std::make_unique<shanks::algos::levin_algorithm<T, K>>(false); })


3. Добавить #include в /include/methods.hpp