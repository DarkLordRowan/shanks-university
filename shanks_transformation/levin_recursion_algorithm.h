/**
 * @file levin_recursion.h
 * @brief This file contains the definition of the Levin recursion transformation class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0


#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library


 /**
 * @brief Levin recursion transformation
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class levin_recursion_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
    * @brief Parameterized constructor to initialize the Levin recursion transformation for series.
    * @param series The series class object
    */
    levin_recursion_algorithm(const series_templ& series);
     /**
    * @brief Levin recursion transformation for series function.
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator() (const K n, const int order) const;

private:
    T operator() (T n_time, T k_time, double b, bool ND) const;
};

template <typename T, typename K, typename series_templ>
levin_recursion_algorithm<T, K, series_templ>::levin_recursion_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T levin_recursion_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;
    else if (order == 0)
        return this->series->S_n(n);

    T beta = -1; // it's an adjustable real parameter that is not a nonpositive integer

    T N_k = (*this)(n, order, beta, 0);
    T D_k = (*this)(n, order, beta, 1);

    return N_k / D_k;
}

template <typename T, typename K, typename series_templ>
T levin_recursion_algorithm<T, K, series_templ>::operator()(T n_time, T k_time, double b, bool ND) const
{
    T R_0 = 0;
    T w_n = pow(T(-1), n_time) * this->series->fact(n_time);

    if (ND == 0)
        R_0 = this->series->S_n(n_time) / w_n;
    else
        R_0 = T(1) / w_n;

    if (k_time == 0)
        return R_0;

    return
    (*this)(n_time + 1, k_time - 1, b, ND) -
    (*this)(n_time, k_time - 1, b, ND) * (b + n_time) * pow((b + n_time + k_time - 1), k_time - 2) /
    pow((b + n_time + k_time), k_time - 1);
}