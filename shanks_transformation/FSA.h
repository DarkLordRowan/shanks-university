/**
 * @file FSA.h
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 */

// TODO удалить к чертям это ускорение
#pragma once

#include "series_acceleration.h" // Include the series header
#include "series.h" 
#include "shanks_transformation.h"

template <typename T, std::unsigned_integral K, typename series_templ>
class ford_sidi_algorithm : public series_acceleration<T, K, series_templ>
{
public:

	/*
	* @brief Parameterized constructor to initialize the Ford-Sidi Algorithm.
    * @authors Matkov N.K. Peters E.A.
    * @param series The series class object to be accelerated
	*/
	ford_sidi_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {shanks_trans = new shanks_transform<T, K, series_templ>(this->series); }
	
	~ford_sidi_algorithm() { delete shanks_trans; }
	/*
	* @brief Fast implementation of Ford-Sidi.
    * @param n The number of terms in the partial sum.
    * @param order The order of transformation.
    * @return The partial sum after the transformation.
	*/
	T operator()(const K n, const K k) const 
	{
		one_series<T, K>* ones_seq = new one_series<T,K>(T(1));

		// Насколько мы коллективным разумом поняли, g это как бы ряд используемого преобразования(shanks_transform, richardson_extrapolation, G_transformation и тд)
		// пока я не реализовал выбор и оставил только shanks_transform, хотя в дальнейшем выбор конечно же надо прикрутить
		
		K k1 = k - 1;
		K i1;

		T T_n_k = Psi(2, k1, (this->series), shanks_trans) - Psi(1, k1, (this->series), shanks_trans);
		T_n_k /= (Psi(2, k1, ones_seq, shanks_trans) - Psi(1, k1, ones_seq, shanks_trans));
		
		for (K i = 2; i <= n; ++i)
		{
			i1 = i + 1;
			T a = Psi(i1, k1, (this->series), shanks_trans) - Psi(i, k1, (this->series), shanks_trans);
			a /= (Psi(i1, k1, ones_seq, shanks_trans) - Psi(i, k1, ones_seq, shanks_trans));
			T_n_k += a;
		}
		return T_n_k;
	}

protected:
	const shanks_transform<T, K, series_templ>* shanks_trans;
	/**
	 * @brief Recursive function to compute psi.
	 * Computes the value of psi according to the given parameters.
	 * @param n The number of terms in the partial sum.
	 * @param k The order of transformation.
	 * @return The value of psi.
	 */
	T Psi(const K n, const K k, const series_base<T,K>* u, const shanks_transform<T, K, series_templ>* g) const
	{
		if (k == 0)
			return (u->operator()(n)) / (g->operator()(n, 1));
		
		K n1 = n + 1;
		K km1 = k - 1;
		K kp1 = k + 1;

		return (Psi(n1, km1, u, g) - Psi(n, km1, u, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
	}
	T Psi(const K n, const K k, const K k_1, const shanks_transform<T, K, series_templ>* g) const
	{
		if (k == 0)
			return (g->operator()(n, k_1)) / (g->operator()(n, 1));
		
		K n1 = n + 1;
		K km1 = k - 1;
		K kp1 = k + 1;

		return (Psi(n1, km1, k_1, g) - Psi(n, km1, k_1, g)) / (Psi(n1, km1, kp1, g) - Psi(n, km1, kp1, g));
	}
};