/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once

#include "../series_acceleration.hpp"
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm"
  //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
  //On a Device for Computing the e (S ) Transformation - nothing new, just matrix
  //euler algoritm A_Note_on_the_Generalised_Euler_Transformation-Wynn-1971 - has Euler, but for um = z^m * v_m
  * @authors Kreinin R.G.
  * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class wynn_epsilon_algorithm_two : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
     * @param series The series class object to be accelerated
     */
    explicit wynn_epsilon_algorithm_two(const series_templ& series);

    /**
     * @brief Fast impimentation of Levin algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see page 20-21 in [https://hal.science/hal-04207550/document]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
wynn_epsilon_algorithm_two<T, K, series_templ>::wynn_epsilon_algorithm_two(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T wynn_epsilon_algorithm_two<T, K, series_templ>::operator()(const K n, const K order) const
{

	using std::isfinite;

	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");
	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	K k = static_cast<K>(2) * order + n;

	std::vector<std::vector<T>> eps(
		4,
		std::vector<T>(
			k + static_cast<K>(1),
			static_cast<T>(0)
		)
	);

	for (K i = static_cast<K>(0); i <= k; ++i)
		eps[3][i] = this->series->S_n(i);


	T a, a1, a2;
	a = a1 = a2 = static_cast<T>(0);

	K i1, i2;

	while (k > static_cast<K>(0))
	{
		for (K i = static_cast<K>(0); i != k; ++i)
		{
			i1 = i + static_cast<K>(1);
			i2 = i + static_cast<K>(2);

			eps[0][i] = eps[2][i1] + static_cast<T>(1) / (eps[3][i1] - eps[3][i]);

			if (!isfinite(eps[0][i]) && i2 <= k) //1 failsafe
			{
				a2 = static_cast<T>(1) / eps[2][i1];

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[2][i2]));
				a = eps[2][i2] * a1;

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[2][i]));
				a += eps[2][i] * a1;

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[0][i2]));
				a -= eps[0][i2] * a1;

				eps[0][i] = static_cast<T>(1) / eps[2][i1];
				eps[0][i] = static_cast<T>(1) / (static_cast<T>(1) + a * eps[0][i]);
				eps[0][i] = eps[0][i] * a;
			}

			if (!isfinite(eps[0][i]))
				eps[0][i] = eps[2][i];

		}

		std::swap(eps[0], eps[1]);
		std::swap(eps[1], eps[2]);
		std::swap(eps[2], eps[3]);

		--k;
	}

	//for k = 0
	std::swap(eps[0], eps[1]);
	std::swap(eps[1], eps[2]);
	std::swap(eps[2], eps[3]);

	if (n % static_cast<K>(2) != static_cast<K>(0))
		return eps[3][0];

	return eps[0][0];

}
