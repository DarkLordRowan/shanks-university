/**
 * @file epsilon_algorithm.h
 * @brief This file contains the declaration of the Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library


 /**
  * @brief Epsilon Algorithm class template.
  * @authors Pashkov B.B.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <typename T, typename K, typename series_templ>
class epsilon_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
   * @brief Parameterized constructor to initialize the Epsilon Algorithm.
   * @authors Pashkov B.B.
   * @param series The series class object to be accelerated
   */
	epsilon_algorithm(const series_templ& series);

	/**
   * @brief Shanks multistep epsilon algorithm.
   * Computes the partial sum after the transformation using the Epsilon Algorithm.
   * For more information, see p. 5.3.2 in [https://e-maxx.ru/bookz/files/numerical_recipes.pdf]  
   * @param n The number of terms in the partial sum.
   * @param order The order of transformation.
   * @return The partial sum after the transformation.
   */
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
epsilon_algorithm<T, K, series_templ>::epsilon_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T epsilon_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
	// computing eps_(2*order)(S_n) as it is Shanks's transformation e_order(S_n) 
	int m = 2 * order;
	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (n == 0)
		return DEF_UNDEFINED_SUM;
	else if (order == 0)
		return this->series->S_n(n);

	std::vector<T> e(m + 1, 0);
	T diff, temp1, temp2;
	for (int j = m; j > 0; --j)
	{
		e[j] = this->series->S_n(n + j);
	}
	temp2 = 0;

	for (int j = m; j > 0; j--)
	{
		temp1 = temp2;
		temp2 = e[j - 1];
		diff = e[j] - temp2;
		if (!std::isfinite(abs(1 / diff)))
			throw std::overflow_error("division by zero");
		e[j - 1] = 1/diff + temp1;
	}

	return e[0];
}