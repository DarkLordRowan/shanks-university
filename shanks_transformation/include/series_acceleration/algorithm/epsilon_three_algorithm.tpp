#pragma once

template<typename T, typename K, typename series_templ>
epsilon_three_algorithm<T, K, series_templ>::
epsilon_three_algorithm(const series_templ &series, T epsilon_threshold_)
    : series_acceleration<T, K, series_templ>(series), epsilon_threshold(epsilon_threshold_) {
}

template<typename T, typename K, typename series_templ>
T epsilon_three_algorithm<T, K, series_templ>::operator()(const K n, const int order) const {
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (n == 0)
        return DEF_UNDEFINED_SUM;

    K N = n;
    T EMACH = std::numeric_limits<T>::epsilon();
    T EPRN = 50 * EMACH;
    T OFRN = std::numeric_limits<T>::max();

    T result = 0, abs_error = 0, resla = 0;
    std::vector<T> e(N + 3, 0);

    for (int i = 0; i <= N; ++i)
        e[i] = this->series->S_n(i);

    for (int i = 0; i <= order; ++i) {
        K num = N, newelm = (N - 1) / 2, K1 = N;
        e[N + 2] = e[N];
        e[N] = abs_error = OFRN;

        for (int I = 1; I <= newelm; ++I) {
            K K2 = K1 - 1, K3 = K1 - 2;
            T RES = e[K1 + 2];
            T E0 = e[K3], E1 = e[K2], E2 = RES;
            T E1ABS = std::abs(E1);
            T DELTA2 = E2 - E1, ERR2 = std::abs(DELTA2);
            T TOL2 = std::max(std::abs(E2), E1ABS) * EMACH;
            T DELTA3 = E1 - E0, ERR3 = std::abs(DELTA3);
            T TOL3 = std::max(E1ABS, std::abs(E0)) * EMACH;

            if (ERR2 > TOL2 || ERR3 > TOL3) {
                T E3 = e[K1];
                e[K1] = E1;
                T DELTA1 = E1 - E3, ERR1 = std::abs(DELTA1);
                T TOL1 = std::max(E1ABS, std::abs(E3)) * EMACH;

                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    N = I + I - 1;
                    break;
                }

                T SS = T(1.0 / DELTA1 + 1.0 / DELTA2 - 1.0 / DELTA3);
                T EPSINF = std::abs(SS * E1);

                if (EPSINF > epsilon_threshold) {
                    RES = T(E1 + 1.0 / SS);
                    e[K1] = RES;
                    K1 -= 2;
                    T ERROR = ERR2 + std::abs(RES - E2) + ERR3;

                    if (ERROR <= abs_error) {
                        abs_error = ERROR;
                        result = RES;
                    }
                } else {
                    N = I + I - 1;
                    break;
                }
            } else {
                result = RES;
                abs_error = ERR2 + ERR3;
                e[K1] = result;
                break;
            }
        }

        if (N == n)
            N = n & 1 ? n : n - 1;

        K ib = num & 1 ? 1 : 2;
        K ie = newelm + 1;

        for (K pos = ib; pos < ib + 2 * ie; pos += 2)
            e[pos] = e[pos + 2];

        if (num != N) {
            K in = num - N + 1;
            for (K j = 1; j <= N; ++j, ++in)
                e[j] = e[in];
        }

        abs_error = std::max(std::abs(result - resla), EPRN * std::abs(result));
        resla = result;
    }

    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}
