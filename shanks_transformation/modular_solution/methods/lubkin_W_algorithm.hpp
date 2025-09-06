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

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	if (order < static_cast<K>(0)) 
		throw std::domain_error("negative order input");

	return calculate(n, order, this->series->S_n(n), 0);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T W_lubkin_algorithm<T, K, series_templ>::calculate(K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	using std::fma;

	/*
	* j - to fix n
	* S_n - partial sum of series.
	*/
	for (K i = 0; i < j; ++i) 
		S_n += this->series->operator()(n  + i + static_cast<K>(1));
	
	n += j;

	if (order == static_cast<K>(0)) return S_n;

	//calculate all basic parts of transfor
	K order1 = order - static_cast<K>(1);
	T W0 = calculate(n, order1, S_n, 0);
	T W1 = calculate(n, order1, S_n, 1);
	T W2 = calculate(n, order1, S_n, 2);
	T W3 = calculate(n, order1, S_n, 3);
	
	//optimization calculations
	T Wo0 = W1 - W0;
	T Wo1 = W2 - W1;
	T Wo2 = W3 - W2;
	T Woo1 = Wo0 * (Wo2 - Wo1);
	T Woo2 = Wo2 * (Wo1 - Wo0);

	//T result = W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1)); //straigh
	const T result = fma(-Wo1, Woo1 / (Woo2 - Woo1), W1); // optimized
	if (!isfinite(result))
		throw std::overflow_error("division by zero");
	return result;
}