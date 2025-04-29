#pragma once

template<typename T, typename K, typename series_templ>
weniger_algorithm<T, K, series_templ>::weniger_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T weniger_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    T numerator = T(0), denominator = T(0);
    T a_n, rest;
    T coef = T(1);

    T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
    T S_n = this->series->S_n(0);

    T rest_a_n;

    // Calculating the coefficient
    for (int m = 0; m < order - 1; ++m)
        coef *= 1 + m;

    for (int j = 0; j <= order; ++j) {
        rest = this->series->minus_one_raised_to_power_n(j) * binomial_coef;
        binomial_coef *= (order - j) / (j + 1);

        rest *= coef;

        coef *= (j + order) / (1 + j);

        a_n = 1 / this->series->operator()(j + 1);

        rest_a_n = rest * a_n;

        numerator += rest_a_n * S_n;

        S_n += this->series->operator()(j + 1);

        denominator += rest_a_n;
    }
    numerator /= denominator;

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}
