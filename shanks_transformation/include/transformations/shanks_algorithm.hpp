/**
 * @file shanks_algorithm.hpp
 * @brief This file contains the definition of the Shanks transformation class
 *        and specialization of this transformation for alternating series.
 */

 // For theory, see:
 // Shanks, D. (1955). Non-linear transformations of divergent and slowly convergent sequences.
 // Journal of Mathematics and Physics, 34(1-4), 1-42.
 // Senhadji, M.N. (2001). On condition numbers of the Shanks transformation.
 // Journal of Computational and Applied Mathematics, 135(1), 41-61.
 // Brezinski, C., He, Y., Hu, X.B., Redivo-Zaglia, M., & Sun, J.Q. (2010).
 // Multistep epsilon-algorithm, Shanks' transformation, and Lotka-Volterra system by Hirota's method.
 // Mathematics of Computation.

#pragma once

#include "series_acceleration.hpp"
#include "wynn_epsilon_1_algorithm.hpp"

/**
 * @brief Shanks transformation for non-alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_algorithm final : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should be non-alternating for optimal performance
	 */
	explicit shanks_algorithm() : series_acceleration<T, K>("shanks original") {};

	/**
	 * @brief Shanks transformation for non-alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(
		const K n, 
        const K order, 
        const series_result<T>& data
	) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_algorithm<T, K>::operator()(
	const K n, 
    const K order, 
    const series_result<T>& data
) const{

	wynn_epsilon_1_algorithm<T, K> prosloyka = wynn_epsilon_1_algorithm<T, K>();

	return prosloyka(n ,2 * order, data);
}