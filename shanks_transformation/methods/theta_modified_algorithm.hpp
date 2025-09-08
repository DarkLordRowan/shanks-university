/**
 * @file theta_modified_algorithm.h
 * @brief This file contains the declaration of the Theta Algorithm class.
 */

#include "series_acceleration.hpp" // Include the series header
#include <vector>

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
	explicit theta_modified_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief modified Theta Algorithm
	 * Computes the partial sum after the transformation using the theta_modified_algorithm
	 * @param n The number of terms in the partial sum.
	 * @param order The order of transformation.
	 * @return The partial sum after the transformation.
	 */

	T operator()(const K n, const K order) const override;
};

#include "theta_modified_algorithm.tpp"