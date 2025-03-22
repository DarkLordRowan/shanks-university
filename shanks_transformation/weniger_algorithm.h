/**
* @file weniger.h
* @brief This files contains the definition of Weniger-transformation
* @authors Yurov P.I. Bezzaborov A.A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <iostream>

template<typename T, typename K, typename series_templ>
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
	* @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
	*/

	weniger_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	* @brief Abstract method for the inherited classes below
	* Computes the partial sum after the transformation using the epsilon Weniger Algorithm.
	* @param k The number of terms in the partial sum.
	* @param n The order of transformation.
	* @return The partial sum after the transformation.
	*/

	T operator()(const K n, const int order) const {

		T numerator = T(0), denominator = T(0);
		T a_n, rest;
		T coef = T(1);


		T binomial_coef = this->series->binomial_coefficient(n, 0);
		T S_n = this->series->S_n(0);

		T rest_a_n;

		for (int m = 0; m < order - 1; ++m) {
			coef *= (1 + m);
		}

		for (int j = 0; j <= order; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j)* binomial_coef;
			binomial_coef = binomial_coef * (order - j) / (j + 1);

			rest = rest * coef;

			coef = coef / (1 + j) * (j + order);

			a_n = 1 / this->series->operator()(j + 1);

			rest_a_n = rest * a_n;

			numerator += rest_a_n * S_n;

			S_n += this->series->operator()(j + 1);

			denominator += rest_a_n;

		}
		numerator /= denominator;
		if (!std::isfinite(numerator)) throw std::overflow_error("division by zero");

		return numerator;
	}
};
