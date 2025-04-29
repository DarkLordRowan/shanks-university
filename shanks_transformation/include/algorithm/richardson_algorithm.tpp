#pragma once

#include "richardson_algorithm.hpp"

template<typename T, typename K, typename series_templ>
richardson_algorithm<T, K, series_templ>::richardson_algorithm(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    if (n == 0)
        return DEF_UNDEFINED_SUM;

    std::vector<std::vector<T> > e(2, std::vector<T>(n + 1, 0));

    for (int i = 0; i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    T a, b;
    a = static_cast<T>(1);

    for (int l = 1; l <= n; ++l) {
        a *= 4;
        b = a - 1;
        for (int m = l; m <= n; ++m)
            e[1][m] = (a * e[0][m] - e[0][m - 1]) / b;

        std::swap(e[0], e[1]);
    }

    const T res = e[n & 1][n]; // n & 1 determines which result to return

    if (!std::isfinite(res))
        throw std::overflow_error("division by zero");

    return res;
}
