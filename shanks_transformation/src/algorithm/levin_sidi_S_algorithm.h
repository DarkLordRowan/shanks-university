/**
* @file levin_sidi_S_algorithm.h
* @brief This file contains implementation of Levin-Sidi S-transformation
* @authors Naumov A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"
#include <vector>

/**
 * @brief S_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 * @param recursive To calculate reccursively
*/
template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm : public series_acceleration<T, K, series_templ> {
protected:
    const T beta;

    const transform_base<T, K> *remainder_func;

    bool recursive;

    /**
    * @brief Default function to calculate S-tranformation directly by formula. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
    * Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
    * @param order The number of terms in the partial sum.
    * @param k the order of transformation
    * @return The partial sum after the transformation.
    */

    virtual T calculate(const K &n, const int &order) const {
        if (order < 0)
            throw std::domain_error("negative integer in input");

        if (beta <= 0)
            throw std::domain_error("beta cannot be initiared by a negative number or a zero");

        T numerator = T(0), denominator = T(0);
        T w_n, rest;
        T up, down;

        T a1, a2, a3, a4, a5;
        a1 = beta + order;
        a2 = a1 + n;

        for (K j = 0; j <= n; ++j) {
            rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(
                       static_cast<T>(n), j);

            up = down = T(1);

            for (K m = 0; m < n - 1; ++m) {
                a3 = a1 + m;
                a4 = a2 + m;
                up *= (a3 + j);
                down *= a4;
            }

            rest *= (up / down);

            w_n = remainder_func->operator()(n, j, this->series, static_cast<T>(beta + n));

            a5 = rest * w_n;

            numerator += a5 * this->series->S_n(order + j);
            denominator += a5;
        }

        numerator /= denominator;

        if (!std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        return numerator;
    }

    /**
    * @brief Default function to calculate S-tranformation using reccurence formula. Implemented u,t and v transformations. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
    * Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
    * @param n The number of terms in the partial sum.
    * @param order the order of transformation
    * @return The partial sum after the transformation.
    */

    T calculate_rec(const K &n, const int &order) const {
        if (order < 0)
            throw std::domain_error("negative integer in input");

        if (beta <= 0)
            throw std::domain_error("beta cannot be initiared by a negative number or a zero");

        std::vector<T> N(n + 1, 0);
        std::vector<T> D(N.size(), 0);

        for (K i = 0; i < K(N.size()); ++i) {
            D[i] = remainder_func->operator()(0, order + i, this->series);
            N[i] = this->series->S_n(order + i) * D[i];
        }

        T b1, b2, b3, b4, b5, b6;
        K j_1;

        b1 = beta + order;
        b2 = b1 - 1;

        for (K i = 1; i <= n; ++i) {
            b3 = b1 + i;
            b4 = b2 + i;
            b5 = b3 + i;
            b6 = b4 + i;
            for (K j = 0; j <= n - i; ++j) {
                T scale1 = ((b3 + j) * (b4 + j));
                T scale2 = (b5 * (b6 + j));

                j_1 = j + 1;

                D[j] = D[j_1] - scale1 * D[j] / scale2;
                N[j] = N[j_1] - scale1 * N[j] / scale2;
            }
        }

        T numerator = N[0] / D[0];

        if (!std::isfinite(numerator))
            throw std::overflow_error("division by zero");

        return numerator;
    }

public:
    /**
    * @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
    * @param series The series class object to be accelerated
    * @param func Remainder function
    * @param recursive How to calculate straightly or reccurently
    */

    levi_sidi_algorithm(const series_templ &series, const transform_base<T, K> *func, bool recursive = false,
                        const T beta_ = T(1)) : series_acceleration<T, K, series_templ>(series), remainder_func(func),
                                                recursive(recursive), beta(beta_) {
    }

    ~levi_sidi_algorithm() { delete remainder_func; }

    /**
   * @brief S-transformation.
   * Computes the partial sum after the S-transformation
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */

    T operator()(const K n, const int order) const {
        if (recursive)
            return calculate_rec(n, order);

        return calculate(n, order);
    }
};
