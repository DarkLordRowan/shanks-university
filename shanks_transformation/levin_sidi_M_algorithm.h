/**
 * @file levin_sidi_M_algorithm.h
 * @brief This file contains the definition of analogues of Levin-Sidi M-transformation
 * @authors Yurov P.I. Bezzaborov A.A.
 */
#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector>
#include <memory> // For std::unique_ptr

/**
 * @brief M_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class M_levin_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
	const T gamma;
	std::unique_ptr<const transform_base<T, K>> remainder_func;

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

	T calculate(const K n, const K order) const {
		if (gamma <= n - 1)
			throw std::domain_error("gamma cannot be lesser than n-1");

		T numerator = T(0), denominator = T(0);
		T w_n, rest;
		T up = T(1), down = T(1);

		T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
		T S_n = this->series->S_n(order);

		T rest_w_n;
		T down_coef = static_cast<T>(gamma + order + 2), up_coef = down_coef - n;

		//TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
		K j1;
		
		for (K m = 0; m < n - 1; ++m) {
			up *= (up_coef + m);
			down *= (down_coef + m);
		}

		up /= down;
		down_coef = static_cast<T>(gamma + order + 1);
		up_coef = (down_coef - n + 1);
		
		for (K j = 0; j <= n; ++j) {
			j1 = j + 1;
			rest = this->series->minus_one_raised_to_power_n(j) * binomial_coef;

			binomial_coef = binomial_coef * (n - j) / j1;

			rest *= up;

			up /= (up_coef + j) * ( down_coef + j);

			w_n = remainder_func->operator()(order, j, this->series, static_cast<T>(-gamma - n));

			rest_w_n = rest * w_n;

			numerator += rest_w_n * S_n ;

			S_n += this->series->operator()(order + j1);

			denominator += rest_w_n;
		}

		numerator /= denominator;

		if (!std::isfinite(numerator))
			throw std::overflow_error("division by zero");

		return numerator;
	}

public:

	/**
	c* @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 * @param series The series class object to be accelerated
	 * @param func Remainder function
	*/

	M_levin_sidi_algorithm(const series_templ& series, const transform_base<T, K>* func, const T gamma_ = T(10)) : series_acceleration<T, K, series_templ>(series), remainder_func(func), gamma(gamma_) {
		if (func == nullptr) 
			throw std::domain_error("null pointer remainder function");
	}

	// Default destructor is sufficient since unique_ptr handles deletion

	/**
     * @brief M-transformation.
     * Computes the partial sum after the M-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

	T operator()(const K n, const K order) const {
		return calculate(n, order);
	}
};