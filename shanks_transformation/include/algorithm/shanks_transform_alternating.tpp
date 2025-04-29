#pragma once

#include "shanks_transform_alternating.hpp"

template<typename T, typename K, typename series_templ>
shanks_transform_alternating<T, K, series_templ>::shanks_transform_alternating(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T shanks_transform_alternating<T, K, series_templ>::operator()(const K n, const int order) const {
    if (n < 0) [[unlikely]]
            throw std::domain_error("negative integer in the input");

    if (order == 0) [[unlikely]]
            return this->series->S_n(n);

    if (n < order || n == 0) [[unlikely]]
            return DEF_UNDEFINED_SUM;

    if (order == 1) [[unlikely]] {
        T a_n, a_n_plus_1, result;
        a_n = this->series->operator()(n);
        a_n_plus_1 = this->series->operator()(n + 1);
        result = std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->series->S_n(n));
        if (!std::isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }

    std::vector<T> T_n(n + order, 0);

    T a_n, a_n_plus_1;
    a_n = this->series->operator()(n - order);
    a_n_plus_1 = this->series->operator()(n - order + 1);
    for (K i = n - order + 1; i <= n + order - 1; ++i) {
        a_n = this->series->operator()(i);
        a_n_plus_1 = this->series->operator()(i + 1);

        T_n[i] = std::fma(a_n * a_n_plus_1, 1 / (a_n - a_n_plus_1), this->series->S_n(n));
    }

    std::vector<T> T_n_plus_1(n + order, 0);
    T a, b, c;
    for (int j = 2; j <= order; ++j) {
        for (K i = n - order + j; i <= n + order - j; ++i) {
            a = T_n[i];
            b = T_n[i - 1];
            c = T_n[i + 1];
            T_n_plus_1[i] = std::fma(std::fma(a, c + b - a, -b * c), 1 / (2 * a - b - c), a);
        }
        T_n = T_n_plus_1;
    }

    if (!isfinite(T_n[n]))
        throw std::overflow_error("division by zero");

    return T_n[n];
}
