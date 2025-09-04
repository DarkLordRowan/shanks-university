/**
 * @file epsilon_modified_algorithm.h
 * @brief This file contains the declaration of the Epsilon Algorithm class.
 */

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_modified_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the Epsilon Algorithm.
	 * @authors Novak.M., Maximov.A.K.
	 * @param series The series class object to be accelerated
	 */
	epsilon_modified_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief modified Epsilon Algorithm
	 * Computes the partial sum after the transformation using the epsilon_modified_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const {

		std::vector<std::vector<T>> eps(n, std::vector<T>(n, 0));

		//std::vector<std::vector<T>> e(2, std::vector<T>(n, 0)); //2 vectors n length containing Epsilon table next and previous 

		for (K i = 0; i < n; ++i)
			eps[i][0] = this->series->operator()(i);

		K m1, k1;

		for (K k = 1; k < n; ++k)
			for (K m = k; m < n; ++m) {
				m1 = m - 1;
				k1 = k - 1;
				eps[m][k] = eps[m1][k1] + T(1) / (eps[m][k1] - eps[m1][k1]) / ((k & 1) ? order + T(k + 1) / 2 : T(2) / (k + 2));

				//eps[m][k] = eps[m1][k1 - 1] + T(1) / (eps[m][k1] - eps[m1][k1]) / ((k & 1) ? order + T(k + 1) / 2 : T(2) / (k + 2));
			}

		if (!std::isfinite(eps[n - 1][n - 1]))
			throw std::overflow_error("division by zero");

		return eps[n - 1][n - 1];
	}
};