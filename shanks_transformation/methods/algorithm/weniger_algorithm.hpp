/**
 * @file weniger.h
 * @brief This file contains the definition of Weniger-transformation
 * @authors Yurov P.I. Bezzaborov A.A.
 */
#pragma once

#include "../series_acceleration.hpp"

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class weniger_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	  * @brief Default function to calculate WT-transformation.
	  * For more information see "Joint use of the Weniger transformation and hyperasymptotics for accurate asymptotic evaluations of a class of saddle-point integrals"
	  * @param k The number of terms in the partial sum.
	  * @param n the order of transformation
	  * @param remainder_func functor, whose returning w_n for t,u or v transformation
  	  * @return The partial sum after the transformation.
	  */

public:
	
	/**
	 * @brief Weniger class template for derivations
	 * @tparam T The type of the elements in the series
	 * @tparam K The type of enumerating integer
	 * @tparam series_templ is the type of series whose convergence we accelerate
	 */

	explicit weniger_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the epsilon Weniger Algorithm.
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T weniger_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T a_n, rest;

	T coef = static_cast<T>(1);
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), static_cast<T>(0));
	T S_n = this->series->S_n(0);

	for (K m = static_cast<K>(0); m < order - static_cast<K>(1); ++m) 
		coef *= static_cast<T>(static_cast<K>(1) + m);
	
	T j1;

	for (K j = static_cast<K>(0); j <= order; ++j) {

		j1 = static_cast<T>(j + static_cast<K>(1));

		rest  = this->series->minus_one_raised_to_power_n(j);
		rest *= binomial_coef;

		binomial_coef *= static_cast<T>(order - j);
		binomial_coef /= j1;

		rest *= coef;

		coef *= static_cast<T>(j + order);
		coef /= j1;

		a_n = static_cast<T>(1);
		a_n/= this->series->operator()(j1);

		rest *= a_n;

		numerator   += rest * S_n;
		denominator += rest;
		S_n += this->series->operator()(j + static_cast<K>(1));
		
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	
	return numerator;
}