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
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class levin_recursion_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
      * @brief Parameterized constructor to initialize the Levin recursion transformation for series.
      * @param series The series class object
      */
    
    // previous standart value is -1.5 
    levin_recursion_algorithm(const series_templ& series, const T beta_ = T(-1.5)) : series_acceleration<T, K, series_templ>(series), beta(beta_) {}
     /**
      * @brief Levin recursion transformation for series function.
      * @param n The number of terms in the partial sum.
      * @param order The order of transformation.
      * @return The partial sum after the transformation.
      */

    T operator()(const K n, const int order) const
    {
        if (n < 0)
            throw std::domain_error("negative integer in the input");

        if (n == 0)
            return DEF_UNDEFINED_SUM;

        if (order == 0)
            return this->series->S_n(n);

        T result = (*this)(n, order, beta, 0);
        result /= (*this)(n, order, beta, 1);

        if (!std::isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }
private:
    const T beta;

    T operator()(K n_time, K k_time, T b, bool ND) const
    {
        T w_n = static_cast<T>(pow(T(-1), n_time) * this->series->fact(n_time));
        T R_0 = (ND == 0 ? this->series->S_n(n_time) : T(1)) / w_n;

        if (k_time == 0)
            return R_0;

        T a1 = k_time - 1;
        T a2 = b + n_time;
        T a3 = a1 + a2;

        T res = static_cast<T>((*this)(n_time + 1, a1, b, ND) -
            (*this)(n_time, a1, b, ND) * a2 * pow(a3, a1 - 1) /
            pow(a3 + 1, a1));

        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }
};