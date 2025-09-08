#pragma once
/**
 * @file epsilon_aitken_theta_algorithm.h
 * @brief This file contains the declaration of the Epsilon-Aitken-Theta Algorithm class.
 */

#include "series_acceleration.hpp" // Include the series header
#include <vector> // Include the vector library

 /**
  * @brief Epsilon-Aitken-Theta Algorithm class template.
  * @tparam T Element type of the series.
  * @tparam K Integer index type.
  * @tparam series_templ Series type to accelerate.
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class epsilon_aitken_theta_algorithm : public series_acceleration<T, K, series_templ>
{
public:
    /**
     * @brief Parameterized constructor to initialize the Theta Algorithm.
     * @authors Novak.M., Maximov.A.K.
     * @param series The series class object to be accelerated
     */
    epsilon_aitken_theta_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief combizing of Epsilon Aitken Theta algorithms
     * Computes the partial sum after the transformation using the Epsilon, Aitken, Theta algo
     * @param n The number of terms in the partial sum.
     * @param order The order of transformation.
     * @return The partial sum after the transformation.
     */

    T operator()(const K n, const K order) const override;
};

#include "epsilon_aitken_theta_algorithm.tpp"