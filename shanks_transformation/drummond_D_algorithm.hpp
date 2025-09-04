/**
* @file drummon_D_algorithm.h
* @brief Contains implementation of Drummond's D-transformation
* @authors Naumov A.
*/

#pragma once

#include "series_acceleration.h"      // Include series_acceleration
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
    char variant = 'u';

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
	* @brief Parameterized constructor to initialize the Drummond's D Algorithm.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
	*/
	explicit drummonds_D_algorithm(const series_templ& series, char variant = 'u', bool useRecFormulas = false);

	/**
    * @brief D-transformation.
    * Computes the partial sum after the D-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const K order) const override;

};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result(const K n, const K order) const {

	T numerator = T(0), denominator = T(0);
	T rest;

	for (K j = 0; j <= n; ++j) {
		rest = this->series->minus_one_raised_to_power_n(j) * this->series->binomial_coefficient(static_cast<T>(n), j) * remainder_func->operator()(n,j, this->series);
		numerator   += rest * this->series->S_n(n+j);
		denominator += rest;
	}

	T result = numerator / denominator;

	if (!std::isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T drummonds_D_algorithm<T,K,series_templ>::calc_result_rec(const K n, const K order) const {

	std::vector<T>   Num(order + 1, 0);
	std::vector<T> Denom(order + 1, 0);

    //init the base values
	for (K i = 0; i < order+1; ++i) {
		Denom[i] = remainder_func->operator()(n, i, this->series);
		  Num[i] = this->series->S_n(n+i) * Denom[i];
	}

    //recurrence
	for (K i = 1; i <= order; ++i)
		for (K j = 0; j <= order - i; ++j) {
			Denom[j] = Denom[j+1] - Denom[j];
			Num[j] = Num[j+1] - Num[j];
		}

	T result = Num[0] / Denom[0];

	if (!std::isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
drummonds_D_algorithm<T,K,series_templ>::drummonds_D_algorithm(const series_templ& series, char variant, bool useRecFormulas) : 
series_acceleration<T, K, series_templ>(series),
useRecFormulas(useRecFormulas)
{
    //check variant else default 'u'
    switch(variant){
        case 't':
            remainder_func.reset(new t_transform<T, K>());
            break;
        case 'v':
            remainder_func.reset(new v_transform<T, K>());
            break;
        case 'd':
            remainder_func.reset(new d_transform<T, K>());
            break;
        case 'w':
            remainder_func.reset(new v_transform_2<T, K>());
            break;
        default:
            remainder_func.reset(new u_transform<T, K>());
            break;
    }
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T drummonds_D_algorithm<T,K,series_templ>::operator()(const K n, const K order) const { 
    T result = (useRecFormulas ? calc_result_rec(n,order) : calc_result(n, order));
    if (!std::isfinite(result)) throw std::overflow_error("division by zero");
    return result;
}
