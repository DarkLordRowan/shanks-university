/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
 * @brief Epsilon Algorithm MK-2 class template.
 * @authors  Kreinin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
	epsilon_algorithm_two(const series_templ& series);

	/**
	* @brief Fast impimentation of Levin algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
epsilon_algorithm_two<T, K, series_templ>::epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const int order) const
{
	int m = 2 * order;

	if (n < 0)
		throw std::domain_error("negative integer in the input");
	else if (n == 0)
		return DEF_UNDEFINED_SUM;
	else if (order == 0)
		return this->series->S_n(n);

	int k = m + n;

	std::vector<std::vector<T>> eps(4, std::vector<T>(k + 1, 0));

	for (int j = k; j >= 0; --j)
	{
		eps[3][j] = this->series->S_n(j);
	}

	T a = 0, a1 = 0, a2 = 0;

	while (k > -1)
	{
		for (int i = 0; i != k; ++i)
		{
			eps[0][i] = eps[2][i + 1] + 1 / (eps[3][i + 1] - eps[3][i]);

			if (!std::isfinite(eps[0][i]) && i + 2 <= k) //1 failsafe
			{
				a2 = 1 / eps[2][i + 1];

				a1 = 1 / (1 - (a2 * eps[2][i + 2]));
				a = eps[2][i + 2] * a1;

				a1 = 1 / (1 - (a2 * eps[2][i]));
				a += eps[2][i] * a1;

				a1 = 1 / (1 - (a2 * eps[0][i + 2]));
				a -= eps[0][i + 2] * a1;

				eps[0][i] = 1 / eps[2][i + 1];
				eps[0][i] = 1 / (1 + a * eps[0][i]);
				eps[0][i] = eps[0][i] * a;
			}
			if (!std::isfinite(eps[0][i]))
			{
				eps[0][i] = eps[2][i];
			}
		}
		std::swap(eps[0], eps[1]);
		std::swap(eps[1], eps[2]);
		std::swap(eps[2], eps[3]);

		--k;
	}

	if (n % 2 != 0)
	{
		return eps[3][0];
	}

	return eps[0][0];
}






