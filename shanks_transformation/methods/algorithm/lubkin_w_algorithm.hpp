/**
 * @file lubkin_w_algorithm.hpp
 * @brief This file contains the definition of Lubkin's W-transformation,
 *        a sequence acceleration method effective for both linear and logarithmic convergence.
 */

 // For theory, see:
 // Lubkin, S. (1952). A method of summing infinite series. J. Res. Nat. Bur. Standards, 48:228-254.
 // Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation. Mathematical Tables and Other Aids to Computation.
 // Osada, N. (1992). A method for obtaining sequence transformations. IMA Journal of Numerical Analysis, 12:85-94.
 // Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. Cambridge University Press. (Chapter 15.4)

#pragma once

#include "../series_acceleration.hpp"
#include <cmath>    // For fma, isfinite
#include <vector>   // For std::vector

/**
 * @brief Lubkin's W-transformation class template implementing a powerful sequence acceleration method.
 *
 * @authors Yurov P.I., Bezzaborov A.A.
 *
 * This algorithm accelerates the convergence of sequences by applying Lubkin's W-transformation,
 * which is particularly effective for both linearly and logarithmically convergent sequences.
 * The transformation is based on a specific rational function representation of the sequence.
 *
 * References:
 * - Lubkin, S. (1952). A method of summing infinite series. J. Res. Nat. Bur. Standards.
 * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * - Osada, N. (1992). A method for obtaining sequence transformations.
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. (Chapter 15.4)
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Determines the arithmetic precision of all computations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations
 *           Typically size_t or unsigned int, must be non-negative
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 *           Represents the mathematical series to be accelerated
 */
template<Accepted T, std::unsigned_integral K, typename series_templ>
class lubkin_w_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	/**
	 * @brief Core implementation of Lubkin's W-transformation.
	 *
	 * For theory, see:
	 * - Lubkin (1952), Eq. (5.2)
	 * - Osada (1992), Section 5
	 * - Sidi (2003), Chapter 15.4, Eq. (15.4.1)
	 * W-transformation formula:
	 * W_n = S_{n+1} - [ΔS_n·ΔS_{n-1}·Δ²S_{n-2}] / [ΔS_n·Δ²S_{n-2} - ΔS_{n-2}·Δ²S_{n-1}]
	 *
	 * @param n The starting index for the transformation
	 * @param order The order of transformation (number of iterations)
	 * @return The accelerated partial sum after Lubkin transformation
	 */
	T calculate(K n, K order);
	
public:

	/**
	 * @brief Parameterized constructor to initialize the Lubkin W-transformation.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 */
	explicit lubkin_w_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

	/**
	 * @brief Applies Lubkin's W-transformation to accelerate series convergence.
	 *
	 * Computes the accelerated sum using Lubkin's W-transformation, which is particularly
	 * effective for sequences with both linear and logarithmic convergence patterns.
	 *
	 * For theory, see: Sidi (2003), Chapter 15.4, Theorem 15.4.1
	 * The transformation accelerates convergence for:
	 * - Logarithmic sequences: Am ∼ A + Σα_i m^{γ-i}
	 * - Linear sequences: Am ∼ A + ζ^m Σα_i m^{γ-i}
	 * - Factorial sequences: Am ∼ A + (ζ^m/(m!)^r) Σα_i m^{γ-i}
	 *
	 * @param n The number of terms to use in the transformation
	 *        Valid values: n ≥ 0 (algorithm requires at least 1 term)
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of iterations)
	 *        Valid values: order ≥ 0
	 *        Higher orders provide more acceleration but require more terms
	 * @return The accelerated partial sum after Lubkin transformation
	 * @throws std::domain_error if negative order is provided
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(K n, K order) override;
};

template<Accepted T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::operator()(const K n, const K order) {

	return calculate(n, order);
}

template<Accepted T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::calculate(K n, const K order) {

	using std::isfinite;
	using std::fma;

	// For theory, see: Wynn (1956), Section 3 - Table construction
	// Storage scheme requires 3n+1 terms for order n transformation
	const K base_size = static_cast<K>(3) * order + static_cast<K>(1);

	std::vector<T> W(
		base_size, 
		static_cast<T>(0)
	);

	for(K i = static_cast<K>(0); i < base_size; ++i){
		W[i] = this->series->Sn(n + i);
	}

	T Wo0, Wo1, Wo2;  // First differences: ΔS_n, ΔS_{n+1}, ΔS_{n+2}
	T Woo1, Woo2;     // Compound terms for denominator calculation

	K j1, j2, j3;     // Index variables

	// For theory, see: 
	// - Lubkin (1952), Eq. (5.2)
	// - Osada (1992), Theorem 2 and Eq. (5.2)
	// - Sidi (2003), Chapter 15.4, Eq. (15.4.1)
	// Iterative application of the W-transformation
	for(K level = static_cast<K>(1); level <= order; ++level){

		for(K j = static_cast<K>(0); j < base_size - level * static_cast<K>(3); ++j){

			j1 = j + static_cast<K>(1);
			j2 = j + static_cast<K>(2);
			j3 = j + static_cast<K>(3);

			// For theory, see: Lubkin (1952), Eq. (5.2)
			// First differences: ΔS_j = S_{j+1} - S_j
			Wo0 = W[j1] - W[j];    // ΔS_j
			Wo1 = W[j2] - W[j1];   // ΔS_{j+1}
			Wo2 = W[j3] - W[j2];   // ΔS_{j+2}

			// For theory, see: Osada (1992), Eq. (5.2)
			// Numerator: (ΔS_n·ΔS_{n-1}·Δ²S_{n-2}) = ΔS_{j+1}·(ΔS_{j+1} - ΔS_j)·ΔS_j
			// Denominator: (ΔS_n·Δ²S_{n-2} - ΔS_{n-2}·Δ²S_{n-1}) = ΔS_{j+2}·(ΔS_{j+1} - ΔS_j) - ΔS_j·(ΔS_{j+2} - ΔS_{j+1})
			Woo1 = Wo0 * (Wo2 - Wo1);  // ΔS_j·(ΔS_{j+2} - ΔS_{j+1})
			Woo2 = Wo2 * (Wo1 - Wo0);  // ΔS_{j+2}·(ΔS_{j+1} - ΔS_j)

			// For theory, see: Lubkin (1952), Main transformation formula
			// W_n = S_{n+1} - [Numerator] / [Denominator]
			// Optimized computation using fused multiply-add for better numerical stability
			W[j] = fma(
				-Wo1, 
				Woo1 / (Woo2 - Woo1),
				 W[j1]
			);
		}
	}

	// Numerical stability check
	if (!isfinite(W[0]))
		throw std::overflow_error("division by zero");
	return W[0];
}