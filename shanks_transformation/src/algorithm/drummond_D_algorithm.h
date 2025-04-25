#pragma once

#include <cmath>
#include "../series_acceleration.h"
#include <vector>

template<typename T, typename K, typename series_templ>
class drummonds_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const transform_base<T, K> *remainder_func;
    bool recursive;
    virtual T calculate(const K &n, const int &order) const {
        if (order < 0)
            throw std::domain_error("negative integer in input");

        T numerator = T(0), denominator = T(0);
        T w_n, rest, a1;

        for (int j = 0; j <= n; ++j) {
            rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(
                       static_cast<T>(n), j);

            w_n = remainder_func->operator()(order, j, this->series, 1);

            a1 = rest * w_n;

            numerator += a1 * this->series->S_n(order + j);
            denominator += a1 * rest;
        }

        if (denominator == 0 || !std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        numerator /= denominator;

        return numerator;
    }

    T calculate_rec(const K &n, const int &order) const {
        if (order < 0)
            throw std::domain_error("negative integer in input");

        std::vector<T> N(n + 1, 0);
        std::vector<T> D(N.size(), 0);

        for (int i = 0; i < n + 1; ++i) {
            D[i] = remainder_func->operator()(0, order + i, this->series);
            N[i] = this->series->S_n(order + i) * D[i];
        }

        for (int i = 1; i <= n; ++i)
            for (int j = 0; j <= n - i; ++j) {
                D[j] = D[j + 1] - D[j];
                N[j] = N[j + 1] - N[j];
            }

        T numerator = N[0] / D[0];

        if (!std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        return numerator;
    }

public:
    drummonds_algorithm(const series_templ &series, const transform_base<T, K> *func,
                        bool recursive = false) : series_acceleration<T, K, series_templ>(series), remainder_func(func),
                                                  recursive(recursive) {
    }

    ~drummonds_algorithm() { delete remainder_func; }

    T operator()(const K n, const int order) const {
        if (recursive)
            return calculate_rec(n, order);

        return calculate(n, order);
    }
};
