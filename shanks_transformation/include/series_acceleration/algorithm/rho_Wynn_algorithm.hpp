#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class rho_Wynn_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const numerator_base<T, K> *numerator_func;
    const T gamma;
    const T RHO;

    // Метод для вычисления
    T calculate(const K n, int order) const;

    // Рекурсивный метод вычисления
    T recursive_calculate_body(const K n, const int order, T S_n, const K j) const;

public:
    // Конструктор
    rho_Wynn_algorithm(const series_templ &series, const numerator_base<T, K> *func, const T gamma_ = T(1), const T RHO_ = T(0));

    // Деструктор
    ~rho_Wynn_algorithm();

    // Оператор вызова
    T operator()(const K n, const int order) const;
};

#include "rho_Wynn_algorithm.tpp"