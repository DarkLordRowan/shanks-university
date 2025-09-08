/**
 * @file levin_algorithm.h
 * @brief This file contains the declaration of the Levin algorithm class.
 */

#pragma once

#include "series_acceleration.hpp" // Include the series header


/**
  * @brief Levin Algorithm class template.
  * @authors  Kreinin R.G., Trudolyubov N.A.
  * @tparam T Element type of the series
  * @tparam K Integer index type
  * @tparam series_templ Series type to accelerate
*/
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_L_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const T beta;
	bool useRecFormulas = false;

	inline T calc_result(K n, K order) const;

	T operator()(
		const K n_time, 
		const K k_time, 
		const T b, 
		const bool ND
	) const;

	inline T calc_result_rec(K n, K order) const;

public:
	/**
	 * @brief Parameterized constructor to initialize the Levin Algorithm.
	 * @param series The series class object to be accelerated
	*/

	explicit levin_L_algorithm(
		const series_templ& series, 
		const bool useRecFormulas = false, 
		const T beta = static_cast<T>(-1.5)
	);

	/**
	 * @brief Fast impimentation of Levin algorithm.
	 * Computes the partial sum after the transformation using the Levin Algorithm.
  	 * For more information, see 3.9.13 in [https://dlmf.nist.gov/3.9]
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	*/

	T operator()(const K n, const K order) const override;
};

#include "levin_L_algorithm.tpp"