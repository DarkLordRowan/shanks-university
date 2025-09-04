/**
 * @file theta_modified_algorithm.h
 * @brief This file contains the declaration of the Theta Algorithm class.
 */

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Theta Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class theta_modified_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	 * @brief Parameterized constructor to initialize the Theta Algorithm.
	 * @authors Novak.M., Maximov.A.K.
	 * @param series The series class object to be accelerated
	 */
	theta_modified_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief modified Theta Algorithm
	 * Computes the partial sum after the transformation using the theta_modified_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const {
		if (n < 2)
			return T(0);

		K m = n - 1;
		K i1, i2;

		T delta, delta_next;

		std::vector<T> current(n), next(n);

		for (K i = 0; i < n; ++i)
			current[i] = this->series->operator()(i);

		for (K k = 0; k < 1; ++k) { 
			for (K i = 0; i < m; ++i) {
				i1 = i + 1;
				i2 = i + 2;

				delta = current[i1] - current[i];

				delta_next = (i2 < n) ? current[i2] - current[i1] : T(0);

				next[i] = current[i1] + (delta * delta_next) / (delta - delta_next);
			}
			current = next;
		}

		for (K i = 0; i < n; ++i)
			std::cout << "[ " << current[i] << " ] ";

		std::cout << "\n";

		const T res = current[m - 1];

		if (!std::isfinite(res))
			throw std::overflow_error("division by zero");

		return res;
	}
};