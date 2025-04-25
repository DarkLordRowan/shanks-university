#pragma once

#include "../series_acceleration.h"

template<typename T, typename K, typename series_templ>
class ford_sidi_algorithm_two : public series_acceleration<T, K, series_templ> {
public:
    ford_sidi_algorithm_two(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
    }

    T operator()(const K n, const int k) const {
        if (n < 0)
            throw std::domain_error("negative integer in the input");

        if (n == 0)
            throw std::domain_error("n = 0 in the input");

        T delta_squared_S_n;

        K m = n;
        do
            delta_squared_S_n = this->series->S_n(m + 2) - 2 * this->series->S_n(m + 1) + this->series->S_n(m);
        while (delta_squared_S_n == 0 && --m > 0);

        if (m == 0)
            throw std::overflow_error("division by zero");

        T delta_S_n = this->series->S_n(m + 1) - this->series->S_n(m);

        T T_n = this->series->S_n(m) - delta_S_n * delta_S_n / delta_squared_S_n;

        if (!isfinite(T_n))
            throw std::overflow_error("division by zero");

        return T_n;
    }
};
