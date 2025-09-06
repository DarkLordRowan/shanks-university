/**
 * @file richardson_algorithm.h
 * @brief This file contains the definition of the Richardson transformation class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
 * @brief Richardson transformation
 * @authors Trudolyubov N.A., Pavlova A.R.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class richardson_algorithm : public series_acceleration<T, K, series_templ>
{
public:
     /**
      * @brief Parameterized constructor to initialize the Richardson transformation for series.
      * @param series The series class object
      */
    explicit richardson_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}
     /**
      * @brief Richardson transformation for series function.
      * @param n The number of terms in the partial sum.
      * @param order The order of transformation.
      * @return The partial sum after the transformation.
      */
    T operator() (const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    // in the method we don't use order, it's only a stub 
    if (n == static_cast<K>(0)) return static_cast<T>(0); // TODO: диагностика

    std::vector<std::vector<T>> e(2, std::vector<T>(n + static_cast<K>(1), static_cast<T>(0))); //2 vectors n + 1 length containing Richardson table next and previous 
    
    for (K i = 0; i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    // The Richardson method main function 
    T a, b;
    a = static_cast<T>(1);
    for (K l = 1; l <= n; ++l) {
        a *= static_cast<T>(4);
        b = a - static_cast<T>(1);

        for (K m = l; m <= n; ++m)
            e[1][m] = fma(a, e[0][m], -e[0][m - 1]) / b;

        std::swap(e[0], e[1]);
    }

    if (!isfinite(e[n & 1][n])) // get n & 1, cause if n is even, result is e[0][n], if n is odd, result is e[1][n]
        throw std::overflow_error("division by zero");

    return e[n & 1][n];
}