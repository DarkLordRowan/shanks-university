/**
 * @file levin_algorithm.h
 * @brief This file contains the declaration of the Levin algorithm class.
 */

#pragma once

#include "series_acceleration.h" // Include the series header


 /**
  * @brief Levin Algorithm class template.
  * @authors  Kreinin R.G.
  * @tparam T Element type of the series
  * @tparam K Integer index type
  * @tparam series_templ Series type to accelerate
  */
template <typename T, std::unsigned_integral K, typename series_templ>
class levin_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 * @param series The series class object to be accelerated
	 */

	levin_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the Levin Algorithm.
  	 * For more information, see 3.9.13 in [https://dlmf.nist.gov/3.9]
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const {
		if (n == 0)
			return T(0);

		if (order == 0)
			return this->series->S_n(n);

		T numerator(0), denominator(0), C_njk, S_nj, g_n, rest;

		for (K j = 0; j <= order; ++j) { //Standart Levin algo procedure
			rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(static_cast<T>(order), j);

			C_njk = static_cast<T>((std::pow((n + j + 1), (order - 1))) / (std::pow((n + order + 1), (order - 1))));

			S_nj = this->series->S_n(n + j);

			g_n = T(1) / (this->series->operator()(n + j));

			rest *= C_njk * g_n;

			denominator += rest;
			numerator += rest * S_nj;
		}
		numerator /= denominator;

		if (!isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}
};