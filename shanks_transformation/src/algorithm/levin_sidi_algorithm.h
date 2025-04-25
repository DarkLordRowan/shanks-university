#pragma once

#include "../series_acceleration.h"
#include <vector>
#include <iostream>

template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class u_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class t_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class v_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class recursive_u_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class recursive_t_levi_sidi_algorithm;

template<typename T, typename K, typename series_templ>
class recursive_v_levi_sidi_algorithm;

class u_transform {
    const T beta;

public:
    u_transform(const T beta_ = T(1)) : beta(beta_) {
    };

    template<typename T, typename K>
    T operator()(const int n, const int j, const series_base<T, K> *series) const {
        return T(1) / ((beta + n) * series->operator()(n + j + 1));
    }
};

class t_transform {
public:
    t_transform() {
    }

    template<typename T, typename K>
    T operator()(const int &n, const int &j, const series_base<T, K> *series) const {
        return T(1) / series->operator()(n + j);
    }
};

class v_transform {
public:
    v_transform() {
    }

    template<typename T, typename K>
    T operator()(const int n, const int j, const series_base<T, K> *series) const {
        return (series->operator()(n + j + 1) - series->operator()(n + j)) /
               (series->operator()(n + j + 1) * series->operator()(n + j));
    }
};

template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const T beta;

    template<class remainderType>
    T calculate(const K k, const int n, remainderType remainder_func) const {
        if (n < 0)
            throw std::domain_error("negative integer in input");

        if (beta <= 0)
            throw std::domain_error("beta cannot be initiared by a negative number or a zero");

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

    template<class remainderType>
    T calculate_recursively(const K k, const int n, remainderType remainder_func) const {
        if (n < 0)
            throw std::domain_error("negative integer in input");

        if (beta <= 0)
            throw std::domain_error("beta cannot be initiared by a negative number or a zero");

        std::vector<T> N(k + 1, 0);
        std::vector<T> D(N.size(), 0);

        for (int i = 0; i < N.size(); ++i) {
            D[i] = remainder_func(0, n + i, this->series);
            N[i] = this->series->S_n(n + i) * D[i];
        }

        T a4, a5, a6, a7, scale1, scale2;
        T j_1;
        for (int i = 1; i <= k; ++i) {
            a4 = beta + n + i;
            a5 = a4 + i;

            for (int j = 0; j <= k - i; ++j) {
                a6 = a4 + j;
                a7 = a5 + j;
                scale1 = (a6 * (a6 - 1));
                scale2 = (a7 * (a7 - 1));

                j_1 = j + 1;

                D[j] = D[j_1] - scale1 * (*D)[j] / scale2;
                N[j] = N[j_1] - scale1 * (*N)[j] / scale2;
            }
        }
        T numerator = N[0] / D[0];

        if (!std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        return numerator;
    }

public:
    friend class u_levi_sidi_algorithm<T, K, series_templ>;
    friend class t_levi_sidi_algorithm<T, K, series_templ>;
    friend class v_levi_sidi_algorithm<T, K, series_templ>;
    friend class recursive_u_levi_sidi_algorithm<T, K, series_templ>;
    friend class recursive_t_levi_sidi_algorithm<T, K, series_templ>;
    friend class recursive_v_levi_sidi_algorithm<T, K, series_templ>;

    levi_sidi_algorithm(const series_templ &series, const T beta_ = T(1)) : series_acceleration<T, K, series_templ>(
                                                                                series), beta(beta_) {
    }

    T operator()(const K k, const int n) const = 0;
};

template<typename T, typename K, typename series_templ>
class u_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    u_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, u_transform{});
    }
};

template<typename T, typename K, typename series_templ>
class t_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    t_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, t_transform{});
    }
};

template<typename T, typename K, typename series_templ>
class v_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    v_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate(k, n, v_transform{});
    }
};

template<typename T, typename K, typename series_templ>
class recursive_u_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    recursive_u_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, u_transform{});
    }
};

template<typename T, typename K, typename series_templ>
class recursive_t_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    recursive_t_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, t_transform{});
    }
};

template<typename T, typename K, typename series_templ>
class recursive_v_levi_sidi_algorithm : public levi_sidi_algorithm<T, K, series_templ> {
public:
    recursive_v_levi_sidi_algorithm(const series_templ &series) : levi_sidi_algorithm<T, K, series_templ>(series) {
    }

    T operator()(const K k, const int n) const {
        return levi_sidi_algorithm<T, K, series_templ>::calculate_recursively(k, n, v_transform{});
    }
};
