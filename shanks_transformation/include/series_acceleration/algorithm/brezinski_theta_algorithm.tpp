#pragma once

template<typename T, typename K, typename series_templ>
brezinski_theta_algorithm<T, K, series_templ>::
brezinski_theta_algorithm(const series_templ &series)
    : series_acceleration<T, K, series_templ>(series) {
}

template<typename T, typename K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::
operator()(const K n, const int order) const {
    if (order & 1 || order < 0) // order is odd or negative
        throw std::domain_error("order should be even number");

    if (n < 0)
        throw std::domain_error("negative integer in the input");

    if (n == 0 || order == 0)
        return this->series->S_n(n);

    return theta(n, order, this->series->S_n(n), 0);
}

template<typename T, typename K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::
theta(K n, const int order, T S_n, const K j) const {
    if (order == 1) {
        T res = T(1) / this->series->operator()(n + j + 1);
        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }

    for (K tmp = n + 1; tmp <= n + j; ++tmp)
        S_n += this->series->operator()(tmp);

    n += j;

    if (order == 0)
        return S_n;

    if (order & 1) // order is odd 
    {
        const T delta = T(1) / (theta(n, order - 1, S_n, 0) - theta(n, order - 1, S_n, 1)); // 1/Δυ_2k^(n)

        if (!std::isfinite(delta))
            throw std::overflow_error("division by zero");

        return theta(n, order - 2, S_n, 1) + delta; // υ_(2k+1)^(n)=υ_(2k-1)^(n+1) + 1/(Δυ_2k^(n)
    }

    // order is even
    const T delta2 = T(1) / (theta(n, order - 1, S_n, 0) - 2 * theta(n, order - 1, S_n, 1) +
                             theta(n, order - 1, S_n, 2)); // Δ^2 υ_(2k+1)^(n)

    if (!std::isfinite(delta2))
        throw std::overflow_error("division by zero");

    const T delta_n = theta(n, order - 2, S_n, 1) - theta(n, order - 2, S_n, 2); // Δυ_2k^(n+1) 
    const T delta_n1 = theta(n, order - 1, S_n, 1) - theta(n, order - 1, S_n, 2); // Δυ_(2k+1)^(n+1)

    return theta(n, order - 2, S_n, 1) + (delta_n * delta_n1) * delta2;
    // υ_(2k+2)^(n)=υ_2k^(n+1)+((Δυ_2k^(n+1))*(Δυ_(2k+1)^(n+1)))/(Δ^2 υ_(2k+1)^(n)
}
