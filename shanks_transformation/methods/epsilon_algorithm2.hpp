/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm"
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix
  //euler algoritm A_Note_on_the_Generalised_Euler_Transformation-Wynn-1971 - has Euler, but for um = z^m * v_m
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
     * @param series The series class object to be accelerated
     */
    explicit epsilon_algorithm_two(const series_templ& series);

    /**
     * @brief Fast impimentation of Levin algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

#include "epsilon_algorithm2.tpp"