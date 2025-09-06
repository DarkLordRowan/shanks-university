/**
 * @file levin_sidi_M_algorithm.h
 * @brief This file contains the definition of analogues of Levin-Sidi M-transformation
 * @authors Yurov P.I. Bezzaborov A.A.
 */
#pragma once

#include "series_acceleration.hpp" // Include the series header
#include "remainders.hpp"
#include <memory> // For std::unique_ptr

/**
 * @brief M_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder type
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_M_algorithm : public series_acceleration<T, K, series_templ>
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

	inline T calculate(const K n, const K order) const;

public:

	/**
	c* @brief Parameterized constructor to initialize the Levin-Sidi M-transformation.
	 * @param series The series class object to be accelerated
	 * @param variant Remainder type
	 * @param gamma_ Parameter //TODO более подробно
	*/

	//levin_sidi_M_algorithm(const series_templ& series, const transform_base<T, K>* func, const T gamma_ = T(10)) : series_acceleration<T, K, series_templ>(series), remainder_func(func), gamma(gamma_)
	explicit levin_sidi_M_algorithm(
		const series_templ& series, 
		const remainder_type variant = remainder_type::u_variant, 
		const T gamma_ = static_cast<T>(10)
	);

	// Default destructor is sufficient since unique_ptr handles deletion

	/**
     * @brief M-transformation.
     * Computes the partial sum after the M-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

	T operator()(const K n, const K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T levin_sidi_M_algorithm<T, K, series_templ>::calculate(const K n, const K order) const {

	using std::isfinite;

	if (gamma - static_cast<T>(n - 1) <= static_cast<T>(0))
		throw std::domain_error("gamma cannot be lesser than n-1");

	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T rest;

	T up = static_cast<T>(1), down = static_cast<T>(1);
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), 0);
	T S_n = this->series->S_n(order);

	T down_coef = gamma + static_cast<T>(order + 2);
	T   up_coef = down_coef - static_cast<T>(n);
	
	for (K m = 0; m < n - 1; ++m) {
		up   *= (up_coef   + static_cast<T>(m));
		down *= (down_coef + static_cast<T>(m));
	}
	up /= down;
	down_coef = gamma + static_cast<T>(order + 1);
	up_coef   = down_coef - static_cast<T>(n + 1);
	
	for (K j = 0; j <= n; ++j) {
		rest  = this->series->minus_one_raised_to_power_n(j); 
		rest *= binomial_coef * static_cast<T>(n - j);
		rest *= up; 
		rest /= static_cast<T>(j + 1);
		up /= (  up_coef + static_cast<T>(j));
		up *= (down_coef + static_cast<T>(j));
		rest *= remainder_func->operator()(
			order, 
			j, 
			this->series, 
			-gamma-static_cast<T>(n)
		);
		numerator += rest * S_n ;
		denominator += rest;
		S_n += this->series->operator()(order + j + 1);
	}

	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	return numerator;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T levin_sidi_M_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {
	return calculate(n, order);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
levin_sidi_M_algorithm<T, K, series_templ>::levin_sidi_M_algorithm(
	const series_templ& series, 
	const remainder_type variant, 
	const T gamma_
	) :
	series_acceleration<T, K, series_templ>(series),
	gamma(gamma_)
{

	switch(variant){
        case remainder_type::u_variant :
            remainder_func.reset(new u_transform<T, K>());
            break;
        case remainder_type::t_variant :
            remainder_func.reset(new t_transform<T, K>());
            break;
        case remainder_type::v_variant :
            remainder_func.reset(new v_transform<T, K>());
            break;
        case remainder_type::t_wave_variant:
            remainder_func.reset(new t_wave_transform<T, K>());
            break;
        case remainder_type::v_wave_variant:
            remainder_func.reset(new v_wave_transform<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
    }
}