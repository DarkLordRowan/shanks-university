/**
 * @file weniger_algorithm.hpp
 * @brief This file contains the definition of the Weniger transformation,
 *        a powerful Levin-type nonlinear sequence transformation for series acceleration.
 * @authors Yurov P.I., Bezzaborov A.A.
 */

 // For theory, see:
 // Weniger, E.J. (1989). Nonlinear sequence transformations for the acceleration of convergence 
 // and the summation of divergent series. Computer Physics Reports, 10(5-6), 189-371.
 // Weniger, E.J. (1992). Interpolation between sequence transformations. Numerical Algorithms, 3(1-4), 477-486.

#pragma once

#include "../series_acceleration.hpp"
#include <cmath> //Include for isfinite

/**
 * @brief Weniger transformation class template implementing a Levin-type acceleration method.
 *
 * This transformation uses factorial series approximations for remainder estimates and is
 * particularly effective for alternating and factorially divergent series. It generalizes
 * Levin's transformation by using Pochhammer symbols instead of powers.
 *
 * References:
 * - Weniger, E.J. (1989). Nonlinear sequence transformations for the acceleration of convergence
 *   and the summation of divergent series. Computer Physics Reports, 10(5-6), 189-371.
 * - Weniger, E.J. (1992). Interpolation between sequence transformations. Numerical Algorithms, 3(1-4), 477-486.
 *
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 *           - T minus_one_raised_to_power_n(K n) const: returns (-1)^n
 *           - T binomial_coefficient(T n, K k) const: returns binomial coefficient C(n, k)
 */
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
class weniger_algorithm final : public series_acceleration<T, K, series_templ>
{
public:
	
	/**
	 * @brief Parameterized constructor to initialize the weniger_algorithm
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 */
	explicit weniger_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Implementation of Weniger transformation for series acceleration.
	 *
	 * Computes the accelerated sum using a Levin-type transformation with factorial series
	 * weights. The algorithm uses binomial coefficients and Pochhammer-like terms for
	 * optimal remainder estimation and convergence acceleration.
	 *
	 * For theory, see: Weniger (1989), Eq. (8.2-7)
	 * Weniger transformation: δ_k^{(n)}(β,s_n) = [Δ^k((β+n)_{k-1}s_n/Δs_n)] / [Δ^k((β+n)_{k-1}/Δs_n)]
	 *
	 * @param n The number of terms to use in the transformation (starting index)
	 *        Valid values: n >= 0
	 * @param order The order of transformation (number of terms in binomial sum)
	 *        Valid values: order >= 1
	 * @return The accelerated partial sum after Weniger transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(K n, K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T weniger_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	using std::isfinite;

	// For theory, see: Weniger (1989), Section 8.2, Eq. (8.2-7)
	// Weniger transformation as ratio of binomial sums with Pochhammer symbols
	T numerator = static_cast<T>(0), denominator = static_cast<T>(0);

	T a_n, rest;

	T coef = static_cast<T>(1);

	// For theory, see: Weniger (1989), Eq. (8.2-7) numerator and denominator structure
	// Initialization of binomial coefficient: C(order, 0) = 1
	T binomial_coef = this->series->binomial_coefficient(static_cast<T>(n), static_cast<K>(0));

	// Initial partial sum S_0
	T S_n = this->series->S_n(0);

	// For theory, see: Weniger (1989), Eq. (8.2-7) weight factors
	// Precomputation of Pochhammer symbol (1)_{order-1} = Γ(order)/Γ(1) = (order-1)!
	for (K m = static_cast<K>(0); m < order - static_cast<K>(1); ++m) 
		coef *= static_cast<T>(static_cast<K>(1) + m);
	
	K j1;

	// For theory, see: Weniger (1989), Eq. (8.2-7) explicit summation form
	// Main summation loop for binomial transformation
	for (K j = static_cast<K>(0); j <= order; ++j) {

		j1 = j + static_cast<K>(1);

		// For theory, see: Weniger (1989), Eq. (8.2-7) term structure
		// Term sign: (-1)^j
		rest  = this->series->minus_one_raised_to_power_n(j);
		// Binomial coefficient: C(order, j)
		rest *= binomial_coef;

		// For theory, see: Weniger (1989), Eq. (8.2-7) recursive binomial update
		// Update binomial coefficient: C(order, j+1) = C(order, j) * (order - j) / (j + 1)
		binomial_coef *= static_cast<T>(order - j);
		binomial_coef /= static_cast<T>(j1);

		// For theory, see: Weniger (1989), Eq. (8.2-7) weight factor
		// Pochhammer symbol factor: (β+n+j)_{k-1} with β=1
		rest *= coef;

		// For theory, see: Weniger (1989), Eq. (8.2-7) recursive Pochhammer update
		// Update Pochhammer-like term: (1+j+order-1)_{k-1} recursive relation
		coef *= static_cast<T>(j + order);
		coef /= static_cast<T>(j1);

		// For theory, see: Weniger (1989), Eq. (8.2-7) remainder estimate
		// Remainder estimate: ω_n = Δs_n = a_{n+1} for Levin-type variants
		a_n = static_cast<T>(1);
		a_n/= this->series->operator()(j1);

		rest *= a_n;

		numerator   += rest * S_n;
		denominator += rest;

		// Update partial sum: S_{j+1} = S_j + a_{j+1}
		S_n += this->series->operator()(j + static_cast<K>(1));
		
	}

	// For theory, see: Weniger (1989), Eq. (8.2-7) final ratio
	// Final transformed value: δ_k^{(n)} = numerator / denominator
	numerator /= denominator;

	if (!isfinite(numerator))
		throw std::overflow_error("division by zero");
	
	return numerator;
}