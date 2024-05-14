/**
 * @file rho-wynn.h
 * @brief This file contains the declaration of the Rho Wynn Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header


 /**
  * @brief Rho Wynn Algorithm class template.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  *
  */
template <typename T, typename K, typename series_templ>
class recursive_rho_Wynn_algorithm : public series_acceleration<T, K, series_templ>
{
protected:
	T recursive_calculate(const K n, const int k) const {
		if (k % 2 != 0) {
			throw std::domain_error("order should be even number");
		}
		if (n < 0 || k < -2)
			throw std::domain_error("negative integer in the input");
		else if (k == 0) {
			return this->series->S_n(n);
		}
		T S_n = this->series->S_n(n);
		T res = recursive_calculate_body(n, k - 2, S_n,1) + T(k) / (recursive_calculate_body(n, k - 1,S_n,1) - recursive_calculate_body(n, k - 1,S_n,0));

		if (!std::isfinite(res)) throw std::overflow_error("division by zero");
		return res;
	}

	T recursive_calculate_body(const K n, const int k, T S_n, const K j) const { 
		/** 
		* S_n - previous sum;
		* j - adjusts n: (n + j);
		*/
		S_n += (j == T(0)) ? T(0) : this->series->operator()(n + j);
		if (k == 0) {
			return S_n;
		}
		else if (k == -1) {
			return 0;
		}
		T res = recursive_calculate_body(n + j, k - 2, S_n,1) + T(k) / (recursive_calculate_body(n + j, k - 1, S_n, 1) - recursive_calculate_body(n + j, k - 1, S_n, 0));
		if (!std::isfinite(res)) throw std::overflow_error("division by zero");
		return res;
	}
public:
	/**
   * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
   * @authors Yurov P.I. Bezzaborov A.A.
   * @param series The series class object to be accelerated
   */
	recursive_rho_Wynn_algorithm(const series_templ& series);
	
	/**
   * @brief Rho Wynn algorithm.
   * Computes the partial sum after the transformation using the Rho Wynn Algorithm.
   * For more information, see 
   * @param n The number of terms in the partial sum.
   * @param k The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const int k) const;
};

template <typename T, typename K, typename series_templ>
recursive_rho_Wynn_algorithm<T, K, series_templ>::recursive_rho_Wynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T recursive_rho_Wynn_algorithm<T, K, series_templ>::operator()(const K n, const int k) const
{
	return recursive_rho_Wynn_algorithm<T, K, series_templ>::recursive_calculate(n, k);
}