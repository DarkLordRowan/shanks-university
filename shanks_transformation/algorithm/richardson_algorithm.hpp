#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class richardson_algorithm final : public series_acceleration<T, K, series_templ> {
public:
    // Конструктор
    explicit richardson_algorithm(const series_templ &series);

    // Оператор вызова
    T operator()(K n, int order) const override;
};

#include "richardson_algorithm.tpp"
