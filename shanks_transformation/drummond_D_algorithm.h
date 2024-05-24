/**
* @file drummon_D_algorithm.h
* @brief Containts implemetation for Drummond's D-transformation
* @authors Naumov A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library



/**
 * @brief D_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 * @param recursive To calculate reccursively
*/
template<typename T, typename K, typename series_templ>
class drummonds_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const transform_base<T, K>* remainder_func;

	bool recursive;

	/**
	* @brief Function to calculate D-tranformation directly by formula. For more information see p. 70 9.5-4 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The number of terms in the partial sum.
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	virtual T calculate(const K& n, const int& order) const {

		if (order < 0)
			throw std::domain_error("negative integer in input");


		T numerator = T(0), denominator = T(0);
		T w_n, rest;

		for (int j = 0; j <= n; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(n, j);

			w_n = remainder_func->operator()(order, j, this->series, 1);

			numerator += rest * this->series->S_n(order + j) * w_n;
			denominator += rest * w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

	/**
	* @brief Function to calculate D-tranformation using reccurence formula. For more information see p. 70 9.5-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param k The number of terms in the partial sum.
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	T calculate_rec(const K& n, const int& order) const {

		if (order < 0)
			throw std::domain_error("negative integer in input");

		std::vector<T>* N = new std::vector<T>(n + 1, 0);
		std::vector<T>* D = new std::vector<T>(n + 1, 0);

		for (int i = 0; i < n + 1; i++) {
			(*D)[i] = remainder_func->operator()(0, order + i, this->series);
			(*N)[i] = this->series->S_n(order + i) * (*D)[i];
		}

		for (int i = 1; i <= n; ++i)
			for (int j = 0; j <= n - i; ++j) {

				(*D)[j] = (*D)[j + 1] - (*D)[j];
				(*N)[j] = (*N)[j + 1] - (*N)[j];
			}

		T numerator = (*N)[0] / (*D)[0];

		delete N, D;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

public:

	/**
	* @brief Parameterized constructor to initialize the Drummonds Algorithm.
	* @param series The series class object to be accelerated
	* @param func Remainder function
	* @param recursive How to calculate
	*/

	drummonds_algorithm(const series_templ& series, const transform_base<T, K>* func, bool recursive = false) : series_acceleration<T, K, series_templ>(series), remainder_func(func), recursive(recursive) {}

	~drummonds_algorithm() { delete remainder_func; }

	/**
   * @brief D-transformation.
   * Computes the partial sum after the D-transformation
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */

	T operator()(const K n, const int order) const {
		if (recursive) return calculate_rec(n, order);
		return calculate(n, order);
	}

};

