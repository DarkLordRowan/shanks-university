/**
 * @file epsilon_DLR.h
 * @brief This file contains the declaration of the Levin algorithm class, Second implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Levin Algorithm class template.
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <typename T, typename K, typename series_templ>
class levin_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Levin Algorithm.
	* @authors Kreinin R.G.
	* @param series The series class object to be accelerated
	*/
	levin_algorithm(const series_templ& series);

	/**
	* @brief Fast impimentation of Levin algorithm.
	* Computes the partial sum after the transformation using the Levin Algorithm.
	* For more information, see 3.9.13 in [https://dlmf.nist.gov/3.9]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int order) const;
};


template <typename T, typename K, typename series_templ>
levin_algorithm<T, K, series_templ>::levin_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T levin_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (n == 0)
		return DEF_UNDEFINED_SUM;
	else if (order == 0)
		return this->series->S_n(n);

	T numerator = 0;
	T denominator = 0;

	for (int j = 0; j < n; j++)
	{
		T rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(order, j);

		T C_jkn_U = std::pow((n + j + 1), (order - 1));
		T C_jkn_L = std::pow((n + order + 1), (order - 1));

		if (!std::isfinite(C_jkn_L))
			throw std::overflow_error("division by zero");

		T C_njk = C_jkn_U / C_jkn_L;

		T S_nj = this->series->S_n(n + j);

		double g_n = this->series->S_n(n + j) - this->series->S_n(n + j - 1);

		if (!std::isfinite(g_n))
			throw std::overflow_error("division by zero");

		numerator += rest * C_njk * S_nj / g_n;
		denominator += rest * C_njk / g_n;
	}
	if (!std::isfinite(denominator))
		throw std::overflow_error("division by zero");

	return numerator / denominator;
}