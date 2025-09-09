/**
 * @file ford_sidi_2_algorithm.hpp
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 *        This implementation is based on the efficient Ford-Sidi algorithm
 *        that requires fewer arithmetic operations than the E-algorithm.
 */

 // For theory, see: 
 // Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
 // Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.

#pragma once

#include "../series_acceleration.hpp"
#include <cmath> //Include for fma, isfinite

/**
 * @brief Ford-Sidi algorithm class template implementing an efficient extrapolation method.
 *
 * @authors Maximov A.K.
 *
 * This algorithm provides a more economical implementation compared to the standard E-algorithm
 * while maintaining mathematical equivalence. It is particularly effective for sequences
 * with specific convergence patterns.
 *
 * References:
 * - Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
 *   SIAM Journal on Numerical Analysis, 24(5), 1212-1232.
 * - Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.
 *   Journal of Computational and Applied Mathematics, 122(1), 223-230.
 *
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_2_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Ford-Sidi V-2 Algorithm.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 */
	explicit ford_sidi_2_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Fast implementation of Ford-Sidi algorithm for series acceleration.
	 *
	 * Computes the accelerated sum using the efficient Ford-Sidi implementation
	 * that requires fewer arithmetic operations than the standard E-algorithm.
	 * The algorithm uses a simplified approach based on finite differences.
	 *
	 * For theory, see: Osada (2000), Section 4, Eq. (20)
	 * T_k^{(n)} = [ψ_{k-1}^{(n+1)}(s) - ψ_{k-1}^{(n)}(s)] / [ψ_{k-1}^{(n+1)}(1) - ψ_{k-1}^{(n)}(1)]
	 *
	 * @param n The number of terms to use in the transformation
	 *        Valid values: n > 0 (algorithm requires at least 1 term)
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (unused in this implementation,
	 *        maintained for interface consistency)
	 *        Valid values: order >= 0 (typically ignored)
	 * @return The accelerated partial sum after Ford-Sidi transformation
	 * @throws std::domain_error if n=0 is provided as input
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(K n, K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_2_algorithm<T, K, series_templ>::operator()(const K n, const K k) const {

	using std::fma;
	using std::isfinite;

	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");

	T delta_squared_S_n;
	K m = n;

	// For theory, see: Osada (2000), Section 2.2
	// The Ford-Sidi algorithm uses auxiliary sequences ψ_k^{(n)}(u) defined via differences
	do{
		// For theory, see: Ford & Sidi (1987), Eq. (1.8)
		// Computation of second difference: Δ²S_m = S_{m+2} - 2S_{m+1} + S_m
		delta_squared_S_n = this->series->S_n(m + static_cast<K>(2));
		delta_squared_S_n-= static_cast<T>(2) * this->series->S_n(m + static_cast<K>(1));
		delta_squared_S_n+= this->series->S_n(m);

	} while (delta_squared_S_n == static_cast<T>(0) && --m > static_cast<K>(0));

	if (m == static_cast<K>(0))
		throw std::overflow_error("division by zero");

	// For theory, see: Ford & Sidi (1987), Eq. (1.9)
	// Computation of first difference: ΔS_m = S_{m+1} - S_m
	T delta_S_n = this->series->S_n(m + static_cast<K>(1));
	delta_S_n -= this->series->S_n(m);

	// For theory, see: Osada (2000), Eq. (20)
	// T_n = S_m - [ (ΔS_m)² / Δ²S_m ]
	const T T_n = fma(-delta_S_n, delta_S_n / delta_squared_S_n, this->series->S_n(m));
	
	if (!isfinite(T_n))
		throw std::overflow_error("division by zero");
	return T_n;
}