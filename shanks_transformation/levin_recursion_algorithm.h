/**
 * @file levin_recursion.h
 * @brief This file contains the definition of the Levin recursion transformation class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header

 /**
 * @brief Levin recursion transformation
 * @authors Trudolyubov N.A.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_recursion_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
      * @brief Parameterized constructor to initialize the Levin recursion transformation for series.
      * @param series The series class object
      */
    
    // previous standart value is -1.5 
    explicit levin_recursion_algorithm(
        const series_templ& series, 
        const T beta_ = static_cast<T>(-1.5)
    );

    //levin_recursion_algorithm(const series_templ& series, const T beta_ = T(-1.5)) : series_acceleration<T, K, series_templ>(series), beta(beta_) {}
     /**
      * @brief Levin recursion transformation for series function.
      * @param n The number of terms in the partial sum.
      * @param order The order of transformation.
      * @return The partial sum after the transformation.
      */

    T operator()(const K n, const K order) const {
        if (n == 0)
            return DEF_UNDEFINED_SUM;

        if (order == 0)
            return this->series->S_n(n);

        const T result = (*this)(n, order, beta, 0) / (*this)(n, order, beta, 1);

        if (!std::isfinite(result))
            throw std::overflow_error("division by zero");

        return result;
    }


private:
    const T beta;

    T operator()(const K n_time, const K k_time, const T b, const bool ND) const {
        T w_n = static_cast<T>(pow(T(-1), n_time) * this->series->fact(n_time));
        T R_0 = (ND == 0 ? this->series->S_n(n_time) : T(1)) / w_n;

        if (k_time == 0)
            return R_0;

        const K a1 = k_time - 1;
        const T a2 = static_cast<T>(b + n_time);
        const T a3 = static_cast<T>(a1 + a2);

        const T res = static_cast<T>(fma(
            -a2 * (*this)(n_time, a1, b, ND),
            pow(a3, a1 - 1) / pow(a3 + 1, a1),
            (*this)(n_time + 1, a1, b, ND)
        ));

        if (!std::isfinite(res))
            throw std::overflow_error("division by zero");

        return res;
    }
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_recursion_algorithm<T, K, series_templ>::levin_recursion_algorithm(
    const series_templ& series, 
    const T beta_
    ) :
    series_acceleration<T, K, series_templ>(series),
    beta(beta_)
{
//TODO: тут нужна ли проверка на допустимость значения бета?
}