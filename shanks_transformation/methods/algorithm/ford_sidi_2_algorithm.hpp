/**
 * @file FSA_two.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

#pragma once

#include "../series_acceleration.hpp"

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi V-2 Algorithm.
	* @authors Maximov A.K.
	* @param series The series class object to be accelerated
	*/
	explicit ford_sidi_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/*
	* @brief Fast implementation of Ford-Sidi.
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm_two<T, K, series_templ>::operator()(const K n, const K k) const {

	using std::fma;
	using std::isfinite;

	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");

	T delta_squared_S_n;
	K m = n;

	do{
		
		delta_squared_S_n = this->series->S_n(m + static_cast<K>(2));
		delta_squared_S_n-= static_cast<T>(2) * this->series->S_n(m + static_cast<K>(1));
		delta_squared_S_n+= this->series->S_n(m);

	} while (delta_squared_S_n == 0 && --m > static_cast<K>(0));

	if (m == static_cast<K>(0))
		throw std::overflow_error("division by zero");

	T delta_S_n = this->series->S_n(m + static_cast<K>(1));
	delta_S_n -= this->series->S_n(m);

	const T T_n = fma(-delta_S_n, delta_S_n / delta_squared_S_n, this->series->S_n(m));
	
	if (!isfinite(T_n))
		throw std::overflow_error("division by zero");
	return T_n;
}