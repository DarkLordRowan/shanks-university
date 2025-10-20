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
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
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
template<std::floating_point T, std::unsigned_integral K, typename series_templ>
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
	T calculate(K n, K order) const;
	
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
	T operator()(K n, K order) const override;
};

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

	return calculate(n, order);
}

template<std::floating_point T, std::unsigned_integral K, typename series_templ>
T lubkin_w_algorithm<T, K, series_templ>::calculate(K n, const K order) const {

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
		W[i] = this->series->S_n(n + i);
	}

	T Wo0, Wo1, Wo2;  // First differences: ΔSᵢ, ΔSᵢ₊₁, ΔSᵢ₊₂
	T Woo1, Woo2;     // Compound terms for denominator calculation
	T denominator;

	K i1, i2, i3;     // Index variables

	// For theory, see: 
	// - Lubkin (1952), Eq. (5.2)
	// - Osada (1992), Theorem 2 and Eq. (5.2)
	// - Sidi (2003), Chapter 15.4, Eq. (15.4.1)
	// Iterative application of the W-transformation
	for(K level = static_cast<K>(1); level <= order; ++level){
		for(K i = static_cast<K>(0); i < base_size - level * static_cast<K>(3); ++i){

			i1 = i + static_cast<K>(1);
			i2 = i + static_cast<K>(2);
			i3 = i + static_cast<K>(3);

			// For theory, see: Lubkin (1952), Eq. (5.2)
			// First differences: ΔSᵢ = Sᵢ₊₁ - Sᵢ
			Wo0 = W[i1] - W[i];    // ΔSᵢ
			Wo1 = W[i2] - W[i1];   // ΔSᵢ₊₁
			Wo2 = W[i3] - W[i2];   // ΔSᵢ₊₂

			// For theory, see: Osada (1992), Eq. (5.2)
			// Numerator: ΔSᵢ₊₁ × (ΔSᵢ₊₁ - ΔSᵢ) × ΔSᵢ
			// Denominator: ΔSᵢ₊₂ × (ΔSᵢ₊₁ - ΔSᵢ) - ΔSᵢ × (ΔSᵢ₊₂ - ΔSᵢ₊₁)
			Woo1 = Wo0 * (Wo2 - Wo1);  // ΔSᵢ × (ΔSᵢ₊₂ - ΔSᵢ₊₁)
			Woo2 = Wo2 * (Wo1 - Wo0);  // ΔSᵢ₊₂ × (ΔSᵢ₊₁ - ΔSᵢ)

			// For theory, see: Lubkin (1952), Main transformation formula
			// Wₙ = Sₙ₊₁ - [Numerator] / [Denominator]
			// Optimized computation using fused multiply-add for better numerical stability
			
			denominator = Woo2 - Woo1;

			// Early exit
			if (abs(denominator) < std::numeric_limits<T>::epsilon()) {
				return W[i1];  // Sₙ₊₁ is the best approximation.
			}

			W[i] = fma(-Wo1, Woo1 / denominator, W[i1]);
		}
	}

	// Numerical stability check
	if (!isfinite(W[0]))
		throw std::overflow_error("division by zero");
	return W[0];
}