/**
 * @file lubkin_w_algorithm.hpp
 * @brief This file contains the definition of Lubkin W-transformation
 * @authors Yurov P.I. Bezzaborov A.A.
 */
#pragma once

#include "../series_acceleration.hpp"
#include <cmath> //Include for fma, isfinite
#include <vector>

/**
 * @brief W_transformation class template.
 * @tparam T The type of the elements in the series
 * @tparam K The type of enumerating integer
 * @tparam series_templ is the type of series whose convergence we accelerate
 */

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class lubkin_w_algorithm final : public series_acceleration<T, K, series_templ>
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

	T calculate(K n, K order) const;
	
public:

	/**
	 * @brief Parameterized constructor to initialize the Lubkin W-transformation.
	 * @param series The series class object to be accelerated
	 * @param func Remainder function
	 */

	explicit lubkin_w_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}


	/**
     * @brief W-transformation.
     * Computes the partial sum after the W-transformation
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

	T operator()(K n, K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	if (order < static_cast<K>(0)) 
		throw std::domain_error("negative order input");

	return calculate(n, order);
}


template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::calculate(K n, const K order) const {

	using std::isfinite;
	using std::fma;

	const K base_size = static_cast<K>(3) * order + static_cast<K>(1);

	std::vector<T> W(
		base_size, 
		static_cast<T>(0)
	);

	for(K i = static_cast<K>(0); i < base_size; ++i){
		W[i] = this->series->S_n(n + i);
	}

	T Wo0, Wo1, Wo2;
	T Woo1, Woo2;


	K j1, j2, j3;

	for(K level = static_cast<K>(1); level <= order; ++level){

		for(K j = static_cast<K>(0); j < base_size - level * static_cast<K>(3); ++j){

			// W[j] = W[j+1] + (W[j+2] - W[j+1]) * (1 - r_(m+1))/(1 - 2 * r_(m+1) + r_m * r_(m+1))
			// r_m = (W[m+2]-W[m+1])/(W[m+1] - W[m])

			j1 = j + static_cast<K>(1);
			j2 = j + static_cast<K>(2);
			j3 = j + static_cast<K>(3);

			//W[j] = W[j1] + 
			//(W[j2]-W[j1]) * (1 - (W[j3]-W[j2])/(W[j2]-W[j1])) / 
			//(1 - 2 * (W[j3]-W[j2])/(W[j2]-W[j1]) + (W[j2]-W[j1])/(W[j1]-W[j])*(W[j3]-W[j2])/(W[j2]-W[j1]));
		
			Wo0 = W[j1] - W[j];
			Wo1 = W[j2] - W[j1];
			Wo2 = W[j3] - W[j2];
			Woo1 = Wo0 * (Wo2 - Wo1);
			Woo2 = Wo2 * (Wo1 - Wo0);

			W[j] = fma(
				-Wo1, 
				Woo1 / (Woo2 - Woo1),
				 W[j1]
			); // optimized <- this is it W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1))

		}
	}
	
	if (!isfinite(W[0]))
		throw std::overflow_error("division by zero");
	return W[0];

}

