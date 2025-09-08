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

#include "richardson_algorithm.tpp"