/**
 * @file epsilon_algorithm_three.h
 * @brief This file contains the declaration of the third implementation of Epsilon Algorithm class.
 */

#pragma once

#include "../series_acceleration.hpp"
#include <vector> // Include the vector library
#undef epsilon

 /**
  * @brief MK-3 Epsilon Algorithm class template.
  * @tparam T Element type of series
  * @tparam K Index type
  * @tparam series_templ Series type to accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_three : public series_acceleration<T, K, series_templ>
{
private:

    const T epsilon_threshold; // Threshold for epsilon, defaults to 1e-3

public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
    explicit epsilon_algorithm_three(const series_templ& series, const T epsilon_threshold_ = static_cast<T>(1e-3));

	/**
	* @brief Fast impimentation of Epsilon algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see 612.zip [https://calgo.acm.org/]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
epsilon_algorithm_three<T, K, series_templ>::epsilon_algorithm_three(
    const series_templ& series, 
    const T epsilon_threshold_
    ) : 
    series_acceleration<T, K, series_templ>(series), 
    epsilon_threshold(epsilon_threshold_) 
{}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T epsilon_algorithm_three<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::max;
    using std::abs;

    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0)) return this->series->S_n(n);

    K N = n; // int -> K

    const T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
    const T EPRN = static_cast<T>(50) * EMACH;
    const T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T 

    T result = static_cast<T>(0); //New result
    T abs_error = static_cast<T>(0); //Absolute error
    T resla = static_cast<T>(0); //Last result
    K newelm, num, NUM, K1, ib, ie, in;
    T RES, E0, E1, E2, E3, E1ABS, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF; // int -> K

    std::vector<T> e(
        N + static_cast<K>(3), 
        static_cast<T>(0)
    ); //First N eliments of epsilon table + 2 elements for math

    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = this->series->S_n(i);

    for (K i = static_cast<K>(0); i <= order; ++i) { //Working with Epsilon Table order times

        num = NUM = K1 = N = n;
        K NEWELM = newelm = (N - static_cast<K>(1)) / static_cast<K>(2);
        e[N + static_cast<K>(2)] = e[N];
        e[N] = abs_error = OFRN;

        for (K I = static_cast<K>(1); I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table

            RES = e[K1 + static_cast<K>(2)];
            E0 = e[K1 - static_cast<K>(2)];
            E1 = e[K1 - static_cast<K>(1)];

            E2 = RES;

            E1ABS = static_cast<T>(abs(E1));

            DELTA2 = E2 - E1;

            ERR2 = static_cast<T>(abs(DELTA2));

            TOL2 = max(
                static_cast<T>(abs(E2)), 
                E1ABS
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;
            ERR3 = static_cast<T>(abs(DELTA3));
            TOL3 = max(
                E1ABS, 
                static_cast<T>(abs(E0))
            );
            TOL3*= EMACH;

            if (ERR2 > TOL2 || ERR3 > TOL3) {

                E3 = e[K1];
                e[K1] = E1;

                DELTA1 = E1;
                DELTA1-= E3;

                ERR1 = static_cast<T>(abs(DELTA1));

                TOL1 = max(
                    E1ABS, 
                    static_cast<T>(abs(E3))
                );
                TOL1*= EMACH;

                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                SS = static_cast<T>(1) / DELTA1 + static_cast<T>(1) / DELTA2 - static_cast<T>(1) / DELTA3;
                EPSINF = static_cast<T>(abs(SS * E1));

                if (EPSINF > epsilon_threshold) {
                    RES = E1 + static_cast<T>(1) / SS;
                    e[K1] = RES;
                    K1 -= static_cast<K>(2);
                    T ERROR = ERR2 + static_cast<T>(abs(RES - E2)) + ERR3;
                    if (ERROR <= abs_error) {
                        abs_error = ERROR;
                        result = RES;
                    }
                }
                else {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
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
            N = (n & 1) ? n : n - static_cast<K>(1);

        ib = (num & static_cast<K>(1)) ? static_cast<K>(1) : static_cast<K>(2);  // Start index: 1 for odd, 2 for even
        ie = newelm + static_cast<K>(1);

        // Copy elements with step 2
        for (K pos = ib; pos < ib + static_cast<K>(2) * ie; pos += static_cast<K>(2))
            e[pos] = e[pos + static_cast<K>(2)];

        if (num != N) {
            in = num - N + static_cast<K>(1);
            for (K j = static_cast<K>(1); j <= N; ++j, ++in)
                e[j] = e[in];
        }

        abs_error = max(
            static_cast<T>(abs(result - resla)), 
            EPRN * static_cast<T>(abs(result))
        );

        resla = result;
    }

    if (!isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}