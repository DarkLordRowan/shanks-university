#pragma once

template<typename T, typename K, typename series_templ>
ford_sidi_two_algorithm<T, K, series_templ>::ford_sidi_two_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T ford_sidi_two_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
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

    if (!std::isfinite(T_n))
        throw std::overflow_error("division by zero");

    return T_n;
}
