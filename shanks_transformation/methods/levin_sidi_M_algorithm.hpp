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

#include "levin_sidi_M_algorithm.tpp"