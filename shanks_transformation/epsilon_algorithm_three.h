/**
 * @file epsilon_algorithm_three.h
 * @brief This file contains the declaration of the third implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library
#undef epsilon

 /**
  * @brief MK-3 Epsilon Algorithm class template.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <typename T, typename K, typename series_templ>
class epsilon_algorithm_three : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
	epsilon_algorithm_three(const series_templ& series);

	/**
	* @brief Fast impimentation of Epsilon algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see 612.zip [https://calgo.acm.org/]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
epsilon_algorithm_three<T, K, series_templ>::epsilon_algorithm_three(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T epsilon_algorithm_three<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;
    else if (order == 0)
        return this->series->S_n(n);

    int N = n;

	T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
	T EPRN = 50 * EMACH; 
	T UFRN = std::numeric_limits<T>::denorm_min() / EPRN; //The smallest finite value of the T
	T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T 

    T result = 0; //New result
    T abs_error = 0; //Absolute error
    T resla = 0; //Last result

    int newelm, num, NUM, K1, K2, K3, ib, ib2, ie, in;
    T RES, E0, E1, E2, E3, E1ABS, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF;

    std::vector<T> e(N + 3, 0); //First N eliments of epsilon table + 2 elements for math

    for(int i = 0; i <= N; ++i) //Filling up Epsilon Table
        e[i] = this->series->S_n(i);

    for (int i = 0; i <= order; ++i) //Working with Epsilon Table order times
    {
        N = n;
        newelm = (N - 1) / 2;
        K NEWELM = (N - 1) / 2;
        e[N + 2] = e[N];
        e[N] = OFRN;
        abs_error = OFRN;
        num = N;
        NUM = N;
        K1 = N;

        for (int I = 1; I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table
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

                SS = 1.0 / DELTA1 + 1.0 / DELTA2 - 1.0 / DELTA3;
                EPSINF = std::abs(SS * E1);

                if (EPSINF > 1e-3) {
                    RES = E1 + 1.0 / SS;
                    e[K1] = RES;
                    K1 -= 2;
                    T ERROR = ERR2 + std::abs(RES - E2) + ERR3;

                    if (ERROR <= abs_error) {
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

        if (N == n) {
            N = 2 * int((n / 2)) - 1;
        }

        ib = 1;
        if (int((num / 2)) * 2 == num) {
            ib = 2;
        }
        ie = newelm + 1;
        for (int j = 1; j <= ie; j++) {
            ib2 = ib + 2;
            e[ib] = e[ib2];
            ib = ib2;
        }

        if (num != N) {
            in = num - N + 1;
            for (int j = 1; j <= N; j++) {
                e[j] = e[in];
                in = in + 1;
            }
        }

        abs_error = std::max(std::abs(result - resla), EPRN * std::abs(result));
        resla = result;
    }


    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}


/*

    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;
    else if (order == 0)
        return this->series->S_n(n);

    int N = 2 * order + n;

    T EMACH = std::numeric_limits<T>::epsilon(); // The smallest relative spacing for the T
    T EPRN = 50 * EMACH;
    T UFRN = std::numeric_limits<T>::denorm_min() / EPRN; //The smallest finite value of the T
    T OFRN = std::numeric_limits<T>::max(); //The largest finite magnitude that can be represented by a T

    T result = 0;
    T abs_error = 0; //Absolute error

    std::vector<T> e(N + 3, 0); //First N eliments of epsilon table + 2 elements for math

    for(int i = 0; i <= N; ++i) //Filling up Epsilon Table
        e[i] = this->series->S_n(i);

    e[N + 2] = e[N];
    int NEWELM = (N - 1) / 2;
    e[N] = OFRN;
    abs_error = OFRN;
    int NUM = N;
    int K1 = N;

    for (int I = 1; I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table
        int K2 = K1 - 1;
        int K3 = K1 - 2;
        T RES = e[K1 + 2];
        T E0 = e[K3];
        T E1 = e[K2];
        T E2 = RES;
        T E1ABS = std::abs(E1);
        T DELTA2 = E2 - E1;
        T ERR2 = std::abs(DELTA2);
        T TOL2 = std::max(std::abs(E2), E1ABS) * EMACH;
        T DELTA3 = E1 - E0;
        T ERR3 = std::abs(DELTA3);
        T TOL3 = std::max(E1ABS, std::abs(E0)) * EMACH;

        if (ERR2 > TOL2 || ERR3 > TOL3) {
            T E3 = e[K1];
            e[K1] = E1;
            T DELTA1 = E1 - E3;
            T ERR1 = std::abs(DELTA1);
            T TOL1 = std::max(E1ABS, std::abs(E3)) * EMACH;

            if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                break;
            }

            T SS = 1.0 / DELTA1 + 1.0 / DELTA2 - 1.0 / DELTA3;
            T EPSINF = std::abs(SS * E1);

            if (EPSINF > 1e-3) {
                RES = E1 + 1.0 / SS;
                e[K1] = RES;
                K1 -= 2;
                T ERROR = ERR2 + std::abs(RES - E2) + ERR3;

                if (ERROR <= abs_error) {
                    abs_error = ERROR;
                    result = RES;
                }
            }
            else {
                break;
            }
        }
        else {
            result = RES;
            abs_error = ERR2 + ERR3;

            if (!std::isfinite(result))
                throw std::overflow_error("division by zero");

            return result;
        }
    }
    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;




*/