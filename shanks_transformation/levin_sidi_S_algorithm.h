/**
* @file levin_sidi_S_algorithm.h
* @brief This files contains implementation of Levin-Sidi S-transformation
* @authors Naumov A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0
#define BETA 1

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

/**
 * @brief S_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 * @param recursive To calculate reccursively
*/
template<typename T, typename K, typename series_templ>
class levi_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const transform_base<T, K>* remainder_func;

	bool recursive;

	/**
	* @brief Default function to calculate S-tranformation directly by formula. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @param n The number of terms in the partial sum.
	* @param k the order of transformation
	* @return The partial sum after the transformation.
	*/

	virtual T calculate(const K& k, const int& n) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (BETA <= 0)
			throw std::domain_error("beta cannot be initiared by a negative number or a zero");


		T numerator = T(0), denominator = T(0);
		T w_n, rest;
		T up, down;

		for (int j = 0; j <= k; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(k, j);

			up = down = T(1);
			for (int m = 0; m < k - 1; ++m) {
				up *= (BETA + n + j + m);
				down *= (BETA + n + k + m);
			}

			rest = rest * (up / down);

			w_n = remainder_func->operator()(n, j, this->series, BETA + n);

			numerator += rest * this->series->S_n(n + j) * w_n;
			denominator += rest * w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

	/**
	* @brief Default function to calculate S-tranformation using reccurence formula. Implemented u,t and v transformations. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @param k The number of terms in the partial sum.
	* @param n the order of transformation
	* @return The partial sum after the transformation.
	*/

	T calculate_rec(const K& k, const int& n) const {

		if (n < 0)
			throw std::domain_error("negative integer in input");
		if (BETA <= 0)
			throw std::domain_error("beta cannot be initiared by a negative number or a zero");

		std::vector<T>* N = new std::vector<T>(k + 1, 0);
		std::vector<T>* D = new std::vector<T>(k + 1, 0);

		for (int i = 0; i < k + 1; i++) {
			(*D)[i] = remainder_func->operator()(0, n + i, this->series);
			(*N)[i] = this->series->S_n(n + i) * (*D)[i];
		}

		for (int i = 1; i <= k; ++i) {
			for (int j = 0; j <= k - i; ++j) {

				T scale1 = ((BETA + n + j + i) * (BETA + n + j + i - 1));
				T scale2 = ((BETA + n + j + 2 * i) * (BETA + n + j + 2 * i - 1));

				(*D)[j] = ((*D)[j + 1] * scale2 - scale1 * (*D)[j]) / scale2;
				(*N)[j] = ((*N)[j + 1] * scale2 - scale1 * (*N)[j]) / scale2;
			}
		}

		T numerator = (*N)[0] / (*D)[0];

		delete N, D;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

public:

	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
	* @param series The series class object to be accelerated
	* @param func Remainder function
	* @param recursive How to calculate straightly or reccurently
	*/

	levi_sidi_algorithm(const series_templ& series, const transform_base<T, K>* func, bool recursive = false) : series_acceleration<T, K, series_templ>(series), remainder_func(func), recursive(recursive) {}

	~levi_sidi_algorithm() { delete remainder_func; }

	/**
   * @brief S-transformation.
   * Computes the partial sum after the S-transformation
   * @param k The number of terms in the partial sum.
   * @param n The order of transformation.
   * @return The partial sum after the transformation.
   */

	T operator()(const K k, const int n) const {
		if (recursive) return calculate_rec(k, n);
		return calculate(k, n);
	}

};
