/**
 * @file epsilon_algorithm_three.h
 * @brief This file contains the declaration of the third implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "../series_acceleration.h"
#include <vector>
#undef epsilon

/**
 * @brief MK-3 Epsilon Algorithm class template.
 * @tparam T Element type of series
 * @tparam K Index type
 * @tparam series_templ Series type to accelerate
 */
template<typename T, typename K, typename series_templ>
class epsilon_algorithm_three : public series_acceleration<T, K, series_templ> {
private:
    T epsilon_threshold; // Threshold for epsilon, defaults to 1e-3

public:
    /**
    * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
    * @param series The series class object to be accelerated
    */
    epsilon_algorithm_three(const series_templ &series, T epsilon_threshold_ = T(1e-3)) : series_acceleration<T, K,
            series_templ>(series), epsilon_threshold(epsilon_threshold_) {
    }

    /**
    * @brief Fast impimentation of Epsilon algorithm.
    * Computes the partial sum after the transformation using the Epsilon Algorithm.
    * For more information, see 612.zip [https://calgo.acm.org/]
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const int order) const {
        if (n < 0)
            throw std::domain_error("negative integer in the input");

        if (n == 0)
            return DEF_UNDEFINED_SUM;

        if (order == 0)
            return this->series->S_n(n);

        K N = n; // int -> K

        T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
        T EPRN = 50 * EMACH;
        T UFRN = std::numeric_limits<T>::denorm_min() / EPRN; //The smallest finite value of the T
        T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T 

        T result = 0; //New result
        T abs_error = 0; //Absolute error
        T resla = 0; //Last result

        K newelm, num, NUM, K1, K2, K3, ib, ie, in;
        T RES, E0, E1, E2, E3, E1ABS, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF;
        // int -> K

        std::vector<T> e(N + 3, 0); //First N eliments of epsilon table + 2 elements for math

        for (int i = 0; i <= N; ++i) //Filling up Epsilon Table
            e[i] = this->series->S_n(i);

        for (int i = 0; i <= order; ++i) //Working with Epsilon Table order times
        {
            num = NUM = K1 = N = n;
            K NEWELM = newelm = (N - 1) / 2;

            e[N + 2] = e[N];
            e[N] = abs_error = OFRN;

            for (int I = 1; I <= NEWELM; ++I) //Counting all diagonal elements of epsilon table
            {
                K2 = K1 - 1;
                K3 = K1 - 2;
                RES = e[K1 + 2];
                E0 = e[K3];
                E1 = e[K2];
                E2 = RES;
                E1ABS = std::abs(E1);
                DELTA2 = E2 - E1;
                ERR2 = std::abs(DELTA2);
                TOL2 = std::max(std::abs(E2), E1ABS) * EMACH;
                DELTA3 = E1 - E0;
                ERR3 = std::abs(DELTA3);
                TOL3 = std::max(E1ABS, std::abs(E0)) * EMACH;

                if (ERR2 > TOL2 || ERR3 > TOL3) {
                    E3 = e[K1];
                    e[K1] = E1;
                    DELTA1 = E1 - E3;
                    ERR1 = std::abs(DELTA1);
                    TOL1 = std::max(E1ABS, std::abs(E3)) * EMACH;

                    if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                        N = I + I - 1;
                        break;
                    }

                    SS = static_cast<T>(1.0 / DELTA1 + 1.0 / DELTA2 - 1.0 / DELTA3);
                    EPSINF = std::abs(SS * E1);

                    if (EPSINF > epsilon_threshold) {
                        RES = static_cast<T>(E1 + 1.0 / SS);
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

            if (N == n) // making N the greatest odd number <= n
                N = (n & 1) ? n : n - 1;

            ib = (num & 1) ? 1 : 2; // Start index: 1 for odd, 2 for even
            ie = newelm + 1;

            // Copy elements with step 2
            for (K pos = ib; pos < ib + 2 * ie; pos += 2)
                e[pos] = e[pos + 2];

            if (num != N) {
                in = num - N + 1;
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
};
