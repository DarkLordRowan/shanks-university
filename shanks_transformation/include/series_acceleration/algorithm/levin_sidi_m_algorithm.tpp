#pragma once

template<typename T, typename K, typename series_templ>
levin_sidi_m_algorithm<T, K, series_templ>::levin_sidi_m_algorithm(
    const series_templ &series,
    const transform_base<T, K> *func,
    const T gamma_
) : series_acceleration<T, K, series_templ>(series), gamma(gamma_) {
    if (func == nullptr)
        throw std::domain_error("null pointer remainder function");

    remainder_func = func;
}

template<typename T, typename K, typename series_templ>
levin_sidi_m_algorithm<T, K, series_templ>::~levin_sidi_m_algorithm() {
    if (remainder_func != nullptr)
        delete remainder_func;
}

template<typename T, typename K, typename series_templ>
T levin_sidi_m_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    return calculate(n, order);
}

template<typename T, typename K, typename series_templ>
T levin_sidi_m_algorithm<T, K, series_templ>::calculate(const K &n, const int &order) const {
    if (order < 0)
        throw std::domain_error("negative integer in input");

    if (gamma <= n - 1)
        throw std::domain_error("gamma cannot be lesser than n-1");

    T numerator = T(0), denominator = T(0);
    T w_n, rest;
    T up = T(1), down = T(1);

    T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
    T S_n = this->series->S_n(order);

    T rest_w_n;
    T down_coef = static_cast<T>(gamma + order + 2), up_coef = down_coef - n;

    for (K m = 0; m < n - 1; ++m) {
        up *= up_coef + m;
        down *= down_coef + m;
    }

    up /= down;
    down_coef = static_cast<T>(gamma + order + 1);
    up_coef = down_coef - n + 1;

    for (K j = 0; j <= n; ++j) {
        rest = this->series->minus_one_raised_to_power_n(j) * binomial_coef;

        binomial_coef = binomial_coef * (n - j) / (j + 1);

        rest *= up;

        up /= (up_coef + j) * (down_coef + j);

        w_n = remainder_func->operator()(order, j, this->series, static_cast<T>(-gamma - n));

        rest_w_n = rest * w_n;

        numerator += rest_w_n * S_n;

        S_n += this->series->operator()(order + j + 1);

        denominator += rest_w_n;
    }

    numerator /= denominator;

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}
