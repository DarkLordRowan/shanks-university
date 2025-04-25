#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"

template<typename T, typename K, typename series_templ>
class levin_algorithm : public series_acceleration<T, K, series_templ> {
public:

    levin_algorithm(const series_templ &series) : series_acceleration<T, K, series_templ>(series) {
    }

    T operator()(const K n, const int order) const {
        if (n < 0)
            throw std::domain_error("negative integer in the input");

        if (n == 0)
            return DEF_UNDEFINED_SUM;

        if (order == 0)
            return this->series->S_n(n);

        T numerator = 0, denominator = 0, C_njk, S_nj, g_n, rest;

        for (int j = 0; j <= order; ++j) //Standart Levin algo procedure
        {
            rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(
                       static_cast<T>(order), j);

            C_njk = static_cast<T>((std::pow((n + j + 1), (order - 1))) / (std::pow((n + order + 1), (order - 1))));

            S_nj = this->series->S_n(n + j);

            g_n = 1 / (this->series->operator()(n + j));

            rest *= C_njk * g_n;

            denominator += rest;
            numerator += rest * S_nj;
        }
        numerator /= denominator;

        if (!std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        return numerator;
    }
};
