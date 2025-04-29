#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class richardson_algorithm : public series_acceleration<T, K, series_templ> {
public:
    // Конструктор
    richardson_algorithm(const series_templ &series);

    // Оператор вызова
    T operator()(const K n, const int order) const;
};

#include "richardson_algorithm.tpp"
