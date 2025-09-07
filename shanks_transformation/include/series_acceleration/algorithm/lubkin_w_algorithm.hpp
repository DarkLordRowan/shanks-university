#pragma once

#include "series_acceleration/series_acceleration.hpp"


template<typename T, typename K, typename series_templ>
class lubkin_w_algorithm final : public series_acceleration<T, K, series_templ> {
protected:
    // Метод для вычисления
    T calculate(K n, int order, T S_n, K j) const;

public:
    // Конструктор
    explicit lubkin_w_algorithm(const series_templ &series);

    // Оператор вызова
    T operator()(K n, int order) const override;
};

#include "lubkin_w_algorithm.tpp"
