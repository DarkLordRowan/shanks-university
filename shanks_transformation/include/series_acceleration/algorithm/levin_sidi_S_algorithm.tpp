#pragma once

template<typename T, typename K, typename series_templ>
levin_sidi_S_algorithm<T, K, series_templ>::levin_sidi_S_algorithm(
    const series_templ &series, const transform_base<T, K> *func,
    const bool recursive, const T beta_
) : series_acceleration<T, K, series_templ>(series),
    beta(beta_), remainder_func(func), recursive(recursive) {}

template<typename T, typename K, typename series_templ>
levin_sidi_S_algorithm<T, K, series_templ>::~levin_sidi_S_algorithm() {
    delete remainder_func;
}

template<typename T, typename K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    if (recursive)
        return calculate_rec(n, order);
    return calculate(n, order);
}

template<typename T, typename K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::calculate(const K &n, const int &order) const {
    if (order < 0)
        throw std::domain_error("negative integer in input");

    if (beta <= 0)
        throw std::domain_error("beta cannot be initiared by a negative number or a zero");

    T numerator = T(0), denominator = T(0);
    T w_n, rest, up, down;
    T a1 = beta + order;
    T a2 = a1 + n;

    for (K j = 0; j <= n; ++j) {
        rest = this->series->minus_one_raised_to_power_n(j) *
               this->series->binomial_coefficient(static_cast<T>(n), j);

        up = down = T(1);
        for (K m = 0; m < n - 1; ++m) {
            T a3 = a1 + m;
            T a4 = a2 + m;
            up *= a3 + j;
            down *= a4;
        }

        rest *= up / down;

        w_n = remainder_func->operator()(n, j, this->series, static_cast<T>(beta + n));
        T a5 = rest * w_n;

        numerator += a5 * this->series->S_n(order + j);
        denominator += a5;
    }

    numerator /= denominator;

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}

template<typename T, typename K, typename series_templ>
T levin_sidi_S_algorithm<T, K, series_templ>::calculate_rec(const K &n, const int &order) const {
    if (order < 0)
        throw std::domain_error("negative integer in input");

    if (beta <= 0)
        throw std::domain_error("beta cannot be initiared by a negative number or a zero");

    std::vector<T> N(n + 1, 0);
    std::vector<T> D(n + 1, 0);

    for (K i = 0; i <= n; ++i) {
        D[i] = remainder_func->operator()(0, order + i, this->series);
        N[i] = this->series->S_n(order + i) * D[i];
    }

    T b1 = beta + order;
    T b2 = b1 - 1;

    for (K i = 1; i <= n; ++i) {
        T b3 = b1 + i;
        T b4 = b2 + i;
        T b5 = b3 + i;
        T b6 = b4 + i;

        for (K j = 0; j <= n - i; ++j) {
            T scale1 = (b3 + j) * (b4 + j);
            T scale2 = b5 * (b6 + j);

            D[j] = D[j + 1] - scale1 * D[j] / scale2;
            N[j] = N[j + 1] - scale1 * N[j] / scale2;
        }
    }

    T numerator = N[0] / D[0];

    if (!std::isfinite(numerator))
        throw std::overflow_error("division by zero");

    return numerator;
}
