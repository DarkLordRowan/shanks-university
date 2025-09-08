/**
 * @file epsilon_algorithm.h
 * @brief This file contains the declaration of the Epsilon Algorithm class.
 */

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm.
     * @authors Pashkov B.B.
     * @param series The series class object to be accelerated
     */
	explicit epsilon_algorithm(const series_templ& series);

	/**
     * @brief Shanks multistep epsilon algorithm.
     * Computes the partial sum after the transformation using the Epsilon Algorithm.
     * For more information, see p. 5.3.2 in [https://e-maxx.ru/bookz/files/numerical_recipes.pdf]
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */
    T operator()(const K n, const K order) const override;
};

#include "epsilon_algorithm.tpp"