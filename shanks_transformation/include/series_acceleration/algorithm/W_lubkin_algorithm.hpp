#pragma once

#include "series_acceleration/series_acceleration.hpp"


template<typename T, typename K, typename series_templ>
class W_lubkin_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    // Метод для вычисления
    T calculate(K n, const int order, T S_n, const K j) const;

public:
    // Конструктор
    W_lubkin_algorithm(const series_templ &series);

    // Оператор вызова
    T operator()(const K n, const int order) const;
};

#include "W_lubkin_algorithm.tpp"