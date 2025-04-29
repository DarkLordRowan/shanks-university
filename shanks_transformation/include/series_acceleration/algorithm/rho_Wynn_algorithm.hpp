#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration/series_acceleration.hpp"

template<typename T, typename K, typename series_templ>
class rho_Wynn_algorithm final : public series_acceleration<T, K, series_templ> {
protected:
    const numerator_base<T, K> *numerator_func;
    const T gamma;
    const T RHO;

    // Метод для вычисления
    T calculate(K n, int order) const;

    // Рекурсивный метод вычисления
    T recursive_calculate_body(K n, int order, T S_n, K j) const;

public:
    // Конструктор
    rho_Wynn_algorithm(const series_templ &series, const numerator_base<T, K> *func, T gamma_ = T(1), T RHO_ = T(0));

    // Деструктор
    ~rho_Wynn_algorithm() override;

    // Оператор вызова
    T operator()(K n, int order) const override;
};

#include "rho_Wynn_algorithm.tpp"