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

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::Psi(
	const K n, 
	const K k, 
	const series_base<T,K>* u, 
	const shanks_transform<T, K, series_templ>* g
) const {
	if (k == static_cast<K>(0))
		return (u->operator()(n)) / (g->operator()(n, static_cast<K>(1)));
	
	K n1  = n + static_cast<K>(1);
	K km1 = k - static_cast<K>(1);
	K kp1 = k + static_cast<K>(1);
	return (Psi(n1, km1, u, g) - Psi(n, km1, u, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::Psi(
	const K n, 
	const K k, 
	const K k_1, 
	const shanks_transform<T, K, series_templ>* g
) const {
	if (k == static_cast<K>(0))
		return (g->operator()(n, k_1)) / (g->operator()(n, static_cast<K>(1)));
	
	K n1  = n + static_cast<K>(1);
	K km1 = k - static_cast<K>(1);
	K kp1 = k + static_cast<K>(1);

	return (Psi(n1, km1, k_1, g) - Psi(n, km1, k_1, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_algorithm<T, K, series_templ>::operator()(const K n, const K k) const 
{
	std::unique_ptr<one_series<T, K>> ones_seq(new one_series<T,K>(1));
	
	// Насколько мы коллективным разумом поняли, g это как бы ряд используемого преобразования(shanks_transform, richardson_extrapolation, G_transformation и тд)
	// пока я не реализовал выбор и оставил только shanks_transform, хотя в дальнейшем выбор конечно же надо прикрутить
	
	K k1 = k - static_cast<K>(1);
	K i1;
	T T_n_k = Psi(static_cast<K>(2), k1, (this->series), shanks_trans.get()) - Psi(static_cast<K>(1), k1, (this->series), shanks_trans.get());
	T_n_k /= (Psi(static_cast<K>(2), k1, ones_seq.get(), shanks_trans.get()) - Psi(static_cast<K>(1), k1, ones_seq.get(), shanks_trans.get()));
	
	for (K i = static_cast<K>(2); i <= n; ++i)
	{
		i1 = i + static_cast<K>(1);
		T a = Psi(i1, k1, (this->series), shanks_trans.get()) - Psi(i, k1, (this->series), shanks_trans.get());
		a /= (Psi(i1, k1, ones_seq.get(), shanks_trans.get()) - Psi(i, k1, ones_seq.get(), shanks_trans.get()));
		T_n_k += a;
	}

	return T_n_k;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
ford_sidi_algorithm<T, K, series_templ>::ford_sidi_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series)
{
	shanks_trans.reset(new shanks_transform<T, K, series_templ>(this->series));
}

