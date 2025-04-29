#pragma once
#include <cmath>

template<typename T, typename K, typename series_templ>
levi_sidi_algorithm<T, K, series_templ>::levi_sidi_algorithm(const series_templ &series, T beta_)
    : series_acceleration<T, K, series_templ>(series), beta(beta_) {
}

template<typename T, typename K, typename series_templ>
template<class remainderType>
T levi_sidi_algorithm<T, K, series_templ>::calculate(K k, int n, remainderType remainder_func) const {
    if (n < 0)
        throw std::domain_error("negative integer in input");

    if (beta <= 0)
        throw std::domain_error("beta cannot be initialized by a negative number or zero");

    T numerator = T(0), denominator = T(0);
    T w_n, rest;
    T up, down;

    T a1, a2, a3;

    for (int j = 0; j <= k; ++j) {
        rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(k, j);
        up = down = T(1);
        a1 = beta + n;
        for (int m = 0; m < k - 1; ++m) {
            a2 = a1 + m;
            up *= (a2 + j);
            down *= (a2 + k);
        }
        rest *= (up / down);

        w_n = remainder_func(n, j, this->series);

        a3 = rest * w_n;
        numerator += a3 * this->series->S_n(n + j);
        denominator += a3;
    }
    numerator /= denominator;

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}

template<typename T, typename K, typename series_templ>
template<class remainderType>
T levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(K k, int n, remainderType remainder_func) const {
    if (n < 0)
        throw std::domain_error("negative integer in input");

    if (beta <= 0)
        throw std::domain_error("beta cannot be initialized by a negative number or zero");

    std::vector<T> N(k + 1, 0);
    std::vector<T> D(k + 1, 0);

    for (int i = 0; i <= k; ++i) {
        D[i] = remainder_func(0, n + i, this->series);
        N[i] = this->series->S_n(n + i) * D[i];
    }

    T a4, a5, a6, a7, scale1, scale2;

    for (int i = 1; i <= k; ++i) {
        a4 = beta + n + i;
        a5 = a4 + i;

        for (int j = 0; j <= k - i; ++j) {
            a6 = a4 + j;
            a7 = a5 + j;
            scale1 = (a6 * (a6 - 1));
            scale2 = (a7 * (a7 - 1));

            D[j] = D[j + 1] - scale1 * D[j] / scale2;
            N[j] = N[j + 1] - scale1 * N[j] / scale2;
        }
    }

    T numerator = N[0] / D[0];

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}
