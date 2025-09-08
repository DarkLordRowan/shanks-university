/**
 * @file lubkin_W_algorithm.h
 * @brief This file contains the definition of Lubkin W-transformation
 * @authors Yurov P.I. Bezzaborov A.A.
 */
#pragma once

#include "series_acceleration.hpp" // Include the series header

/**
 * @brief W_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class W_lubkin_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	 * @brief Default function to calculate W-tranformation. 
	 * For more information see p. 290 15.4.1 [http://servidor.demec.ufpr.br/CFD/bibliografia/MER/Sidi_2003.pdf]
	 * @authors Yurov P.I. Bezzaborov A.A.
	 * @param n The number of terms in the partial sum.
	 * @param order the order of transformation
	 * @return The partial sum after the transformation.
	 */

	T calculate(K n, const K order, T S_n, const K j) const;
	
public:

	/**
	 * @brief Parameterized constructor to initialize the Lubkin W-transformation.
	 * @param series The series class object to be accelerated
	 * @param func Remainder function
	 */

	explicit W_lubkin_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}


	/**
     * @brief W-transformation.
     * Computes the partial sum after the W-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

	T operator()(const K n, const K order) const override;
};

#include "lubkin_W_algorithm.tpp"