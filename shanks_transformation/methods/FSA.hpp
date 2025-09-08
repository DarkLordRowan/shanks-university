/**
 * @file FSA.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

// TODO удалить к чертям это ускорение
#pragma once

#include "series_acceleration.hpp" // Include the series header
#include "../series.h" 
#include "shanks_transformation.hpp"

#include <memory>

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
protected:

	std::unique_ptr<const shanks_transform<T, K, series_templ>> shanks_trans;

	/**
	 * @brief Recursive function to compute psi.
	 * Computes the value of psi according to the given parameters.
	 * @param n The number of terms in the partial sum.
	 * @param k The order of transformation.
	 * @return The value of psi.
	 */

	T Psi(
		const K n, 
		const K k, 
		const series_base<T,K>* u, 
		const shanks_transform<T, K, series_templ>* g
	) const;

	T Psi(
		const K n, 
		const K k, 
		const K k_1, 
		const shanks_transform<T, K, series_templ>* g
	) const;

public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi Algorithm.
    * @authors Matkov N.K. Peters E.A.
    * @param series The series class object to be accelerated
	*/
	explicit ford_sidi_algorithm(const series_templ& series);
	
	/*
	* @brief Fast implementation of Ford-Sidi.
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K k) const override;
};

#include "FSA.tpp"