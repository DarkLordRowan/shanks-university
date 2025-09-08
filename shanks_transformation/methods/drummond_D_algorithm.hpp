/**
* @file drummon_D_algorithm.h
* @brief Contains implementation of Drummond's D-transformation
* @authors Naumov A.
*/

#pragma once

#include "series_acceleration.hpp"    // Include series_acceleration
#include "remainders.hpp"  		      // Include remainders
#include <memory>					  // Include for unique ptr

/**
 * @brief D_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 * @param remainder_func - remainder to use in formula
 * @param useRecFormulas - use reccurence relation, the calculation will be done using arrays
 * @param variant        - type of remainder to use
*/

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class drummonds_D_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

    std::unique_ptr<const transform_base<T, K>> remainder_func;
    bool useRecFormulas = false;
    remainder_type variant = remainder_type::u_variant;

	/**
	* @brief Function to calculate D-tranformation directly by formula. For more information see p. 70 9.5-4 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	inline T calc_result(const K n, const K order) const;

	/**
	* @brief Function to calculate D-tranformation using reccurence relation. For more information see p. 70 9.5-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/

	inline T calc_result_rec(const K n, const K order) const;

public:

	/**
	* @brief Parameterized explicit constructor to initialize the Drummond's D Algorithm.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
	*/

	explicit drummonds_D_algorithm(
		const series_templ& series, 
		const remainder_type variant = remainder_type::u_variant, 
		bool useRecFormulas = false
	);

	/**
    * @brief D-transformation.
    * Computes the partial sum after the D-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */

    T operator()(const K n, const K order) const override;

};

#include "drummond_D_algorithm.tpp"