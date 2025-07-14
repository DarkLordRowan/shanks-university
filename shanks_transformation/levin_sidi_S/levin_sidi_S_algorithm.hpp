/**
* @file levin_sidi_S_algorithm.hpp
* @brief Contains implementation of Drummond's D-transformation
* @authors Naumov A.
*/

#pragma once

#include "../remainders/remainders.hpp" // Include remainders
#include "../series_acceleration.h"     // Include series_acceleration

/**
 * @brief S_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 * @param beta           - nonzero positive parameter, 1 is the standart value, for more information see p. 39 in [https://arxiv.org/pdf/math/0306302.pdf]
 * @param remainder_func - remainder to use in calculations
 * @param useRecFormulas - use reccurence relation, the calculation will be done using arrays
 * @param variant        - type of remainder to use
*/
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class levin_sidi_S_algorithm : public series_acceleration<T, K, series_templ> {
protected:

    T beta = T(1);
    transform_base<T, K>* remainder_func;
    bool useRecFormulas = false;
    char variant = 'u';

    /**
	* @brief Function to calculate S-tranformation directly by formula. For more information see p. 57 8.2-7 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/
    inline T calc_result(K n, K order) const;

    /**
	* @brief Function to calculate S-tranformation using reccurence relation. For more information see p. 57 8.3-5 [https://arxiv.org/pdf/math/0306302.pdf]
	* @param n The partial sum number (S_n) from which the calculations will be done
	* @param order the order of transformation
	* @return The partial sum after the transformation.
	*/
    inline T calc_result_rec(K n, K order) const;

public:

    /**
	* @brief Parameterized constructor to initialize the Levin-Sidi S-transformation.
	* @param series The series class object to be accelerated
	* @param variant Type of remainder to use
	* @param useRecFormulas use reccurence or straightforward formula 
    * @param parameter value of beta parameter
	*/
    explicit levin_sidi_S_algorithm(const series_templ& series, char variant = 'u', bool useRecFormulas = false,  T parameter = T(1));

    /**
    * @brief S-transformation.
    * Computes the partial sum after the S-transformation
    * @param n The partial sum number (S_n) from which the calculations will be done
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
    */
    T operator()(const K n, const K order) const override;

    ~levin_sidi_S_algorithm() { delete remainder_func;}
};

#include "levin_sidi_S_algorithm.tpp"