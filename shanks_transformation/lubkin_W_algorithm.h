/**
* @file lubkin_W_algorithm.h
* @brief This files contains the definition of Lubkin W-transformation
* @authors Yurov P.I. Bezzaborov A.A.
*/
#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector>

/**
 * @brief W_transformation class template.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
*/

template<typename T, typename K, typename series_templ>
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

	T calculate(K n, const int& order, T S_n, const K& j) const {
		/*
		* j - to fix n
		* S_n - partial sum of series.
		*/
		for (K i = 0; i < j; i++) {
			S_n += this->series->operator()(n + 1 + i);
		}
		n += j;
		if (order == 0) return S_n;
		//calculate all basic parts of transform
		T W0 = calculate(n, order - 1, S_n, 0);
		T W1 = calculate(n, order - 1, S_n, 1);
		T W2 = calculate(n, order - 1, S_n, 2);
		T W3 = calculate(n, order - 1, S_n, 3);
		
		//optimization calculations
		T Wo0 = (W1 - W0);
		T Wo1 = (W2 - W1);
		T Wo2 = (W3 - W2);
		T Woo1 = Wo0 * (Wo2 - Wo1);
		T Woo2 = Wo2 * (Wo1 - Wo0);

		//T result = W1 - ((W2 - W1) * (W1 - W0) * (W3 - 2 * W2 + W1)) / ((W3 - W2) * (W2 - 2 * W1 + W0) - (W1 - W0) * (W3 - 2 * W2 + W1)); //straigh
		T result = W1 - (Wo1 * Woo1) / (Woo2 - Woo1); // optimized

		if (!std::isfinite(result))
			throw std::overflow_error("division by zero");
		return result;

	}
public:

	/**
	* @brief Parameterized constructor to initialize the Lubkin W-transformation.
	* @param series The series class object to be accelerated
	* @param func Remainder function
	*/

	W_lubkin_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}


	/**
   * @brief W-transformation.
   * Computes the partial sum after the W-transformation
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */

	T operator()(const K n, const int order) const {
		if (order < 0) throw std::domain_error("negative order input");
		T S_n = this->series->S_n(n);
		return calculate(n, order, S_n,0);
	}
};
