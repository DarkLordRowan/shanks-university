/**
* @file levin_sidi_M_algorithm.h
* @brief This files contains the definition of analogues of Levin-Sidi M-transformation
* @authors Yurov P.I. Bezzaborov A.A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0
#define GAMMA 10 // gamma is a a nonzero positive parameter, 10 is chosen by default

#include "series_acceleration.h" // Include the series header
#include <vector>

/**
 * @brief M_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
*/


template<typename T, typename K, typename series_templ>
class M_levin_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	const transform_base<T, K>* remainder_func;

	/**
	* @brief Default function to calculate M-transformation. Implemented u,t,d and v transformations. For more information see p. 65 9.2-6 [https://arxiv.org/pdf/math/0306302.pdf]
	* Levin-Sidi or Factorial analog of Levin Transformation is effective for series that belong to b(1)/LIN/FAC and inferior on b(1)/LOG for more information see p. 369 and p.285 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	* @authors Yurov P.I. Bezzaborov A.A.
	* @param n The number of terms in the partial sum.
	* @param order the order of transformation
	* @param remainder_func functor, whose returning w_n for u,t,d or v transformation
	* @return The partial sum after the transformation.
	* We assume that the Pochhammer symbol satisfies (-x)_n = (-1)^n*(x-n+1)_n
	*/


	T calculate(const K& n, const int& order) const {

		if (order < 0)
			throw std::domain_error("negative integer in input");
		if (GAMMA <= n-1)
			throw std::domain_error("gamma cannot be lesser than n-1");


		T numerator = T(0), denominator = T(0);
		T w_n, rest;
		T up = T(1), down = T(1);

		T binomial_coef = this->series->binomial_coefficient(n, 0);
		T S_n = this->series->S_n(order);

		T rest_w_n;
		T down_coef = (GAMMA + order + 2), up_coef = down_coef - n;
		
		for (int m = 0; m < n - 1; ++m) {
			up *= (up_coef + m);
			down *= (down_coef + m);
		}

		up = (up / down);
		down_coef = (GAMMA + order + 1);
		up_coef = (down_coef - n + 1);
		
		for (int j = 0; j <= n; ++j) {

			rest = this->series->minus_one_raised_to_power_n(j) * binomial_coef;

			binomial_coef = binomial_coef * (n - j) / (j + 1);

			rest = rest * up;

			up = up / (up_coef + j) * ( down_coef + j );

			w_n = remainder_func->operator()(order, j, this->series, -GAMMA-n);

			rest_w_n = rest * w_n;

			numerator += rest_w_n * S_n ;

			S_n += this->series->operator()(order + j + 1);

			denominator += rest_w_n;

		}

		numerator /= denominator;

		if (!std::isfinite(numerator)) throw std::overflow_error("division by zero");

		return numerator;
	}


public:

	/**
	* @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	* @param series The series class object to be accelerated
	* @param func Remainder function
	*/

	M_levin_sidi_algorithm(const series_templ& series, const transform_base<T, K>* func) : series_acceleration<T, K, series_templ>(series) {
		if (func == nullptr) throw std::domain_error("null pointer remainder function");
		remainder_func = func;
	}

	~M_levin_sidi_algorithm() { if(remainder_func != nullptr) delete remainder_func; }

	/**
   * @brief M-transformation.
   * Computes the partial sum after the M-transformation
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */

	T operator()(const K n, const int order) const {
		return calculate(n, order);
	}

};

