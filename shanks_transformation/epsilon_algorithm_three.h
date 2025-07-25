/**
 * @file epsilon_algorithm_three.h
 * @brief This file contains the declaration of the third implementation of Epsilon Algorithm class.
 */

#pragma once

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library
#undef epsilon

 /**
  * @brief MK-3 Epsilon Algorithm class template.
  * @tparam T Element type of series
  * @tparam K Index type
  * @tparam series_templ Series type to accelerate
  */
template <typename T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_three : public series_acceleration<T, K, series_templ>
{
private:
    T epsilon_threshold; // Threshold for epsilon, defaults to 1e-3

public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
    epsilon_algorithm_three(const series_templ& series, const T epsilon_threshold_ = T(1e-3)) : series_acceleration<T, K, series_templ>(series), epsilon_threshold(epsilon_threshold_) {}

	/**
	* @brief Fast impimentation of Epsilon algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see 612.zip [https://calgo.acm.org/]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K order) const {
        if (n == 0)
            return T(0);

        if (order == 0)
            return this->series->S_n(n);

        K N = n; // int -> K

        const T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
        const T EPRN = T(50) * EMACH;
        const T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T 

        T result(0); //New result
        T abs_error(0); //Absolute error
        T resla(0); //Last result

        K newelm, num, NUM, K1, ib, ie, in;
        T RES, E0, E1, E2, E3, E1ABS, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF; // int -> K

        std::vector<T> e(N + 3, T(0)); //First N eliments of epsilon table + 2 elements for math

        for (K i = 0; i <= N; ++i) //Filling up Epsilon Table
            e[i] = this->series->S_n(i);

        for (K i = 0; i <= order; ++i) { //Working with Epsilon Table order times
            num = NUM = K1 = N = n;
            K NEWELM = newelm = (N - 1) / 2;

            e[N + 2] = e[N];
            e[N] = abs_error = OFRN;

            for (K I = 1; I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table
                RES = e[K1 + 2];
                E0 = e[K1 - 2];
                E1 = e[K1 - 1];
                E2 = RES;
                E1ABS = abs(E1);
                DELTA2 = E2 - E1;
                ERR2 = abs(DELTA2);
                TOL2 = T(max(abs(E2), abs(E1))) * EMACH;
                DELTA3 = E1 - E0;
                ERR3 = abs(DELTA3);
                TOL3 = T(max(abs(E1), abs(E0))) * EMACH;

                if (ERR2 > TOL2 || ERR3 > TOL3) {
                    E3 = e[K1];
                    e[K1] = E1;
                    DELTA1 = E1 - E3;
                    ERR1 = abs(DELTA1);
                    TOL1 = T(max(abs(E1), abs(E3))) * EMACH;

                    if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                        N = I + I - 1;
                        break;
                    }

                    SS = T(1.0) / DELTA1 + T(1.0) / DELTA2 - T(1.0) / DELTA3;
                    EPSINF = abs(SS * E1);

                    if (EPSINF > epsilon_threshold) {
                        RES = static_cast<T>(E1 + T(1.0) / SS);
                        e[K1] = RES;
                        K1 -= 2;
                        T ERROR = ERR2 + T(abs(RES - E2)) + ERR3;

                        if (abs(ERROR) <= abs(abs_error)) {
                            abs_error = ERROR;
                            result = RES;
                        }
                    }
                    else {
                        N = I + I - 1;
                        break;
                    }
                }
                else {
                    result = RES;
                    abs_error = ERR2 + ERR3;
                    e[K1] = result;
                    break;
                }
            }

            if (N == n) // making N the greatest odd number <= n
                N = (n & 1) ? n : n - 1;

            ib = (num & 1) ? 1 : 2;  // Start index: 1 for odd, 2 for even
            ie = newelm + 1;

            // Copy elements with step 2
            for (K pos = ib; pos < ib + 2 * ie; pos += 2)
                e[pos] = e[pos + 2];

            if (num != N) {
                in = num - N + 1;
                for (K j = 1; j <= N; ++j, ++in)
                    e[j] = e[in];
            }

            abs_error = max(abs(result - resla), abs(EPRN * T(abs(result))));
            resla = result;
        }

        if (!isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }
};