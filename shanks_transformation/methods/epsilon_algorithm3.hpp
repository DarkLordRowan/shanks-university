/**
 * @file epsilon_algorithm_three.h
 * @brief This file contains the declaration of the third implementation of Epsilon Algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header
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

#include "epsilon_algorithm3.tpp"