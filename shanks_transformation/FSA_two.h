/**
 * @file FSA_two.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

#pragma once

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library
#include "series.h" 

template <typename T, typename K, typename series_templ>
class ford_sidi_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi V-2 Algorithm.
	* @authors Maximov A.K.
	* @param series The series class object to be accelerated
	*/
	ford_sidi_algorithm_two(const series_templ& series);

	/*
	* @brief Fast implementation of Ford-Sidi.
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int k) const;
};

template <typename T, typename K, typename series_templ>
ford_sidi_algorithm_two<T, K, series_templ>::ford_sidi_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T ford_sidi_algorithm_two<T, K, series_templ>::operator()(const K n, const int order) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	if (n == 0)
		throw std::domain_error("n = 0 in the input");

	T delta_squared_S_n;

	K m = n;
	do
	{
		delta_squared_S_n = this->series->S_n(m + 2) - 2 * this->series->S_n(m + 1) + this->series->S_n(m);
		m = m - 1;
	} while (delta_squared_S_n == 0 && m > 0);

	if (m == 0)
		throw std::overflow_error("division by zero");

	T delta_S_n = this->series->S_n(m + 1) - this->series->S_n(m);

	T T_n = this->series->S_n(m) - delta_S_n * delta_S_n / delta_squared_S_n;

	if (!isfinite(T_n))
		throw std::overflow_error("division by zero");

	return T_n;
}