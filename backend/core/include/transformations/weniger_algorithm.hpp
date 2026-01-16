#ifndef WENIGER_ALGORITHM_HPP
#define WENIGER_ALGORITHM_HPP
#pragma once
/**
 * @file weniger_algorithm.hpp
 * @brief This file contains the definition of the Weniger transformation,
 *        a powerful Levin-type nonlinear sequence transformation for series acceleration.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

 // For theory, see:
 // Weniger, E.J. (1989). Nonlinear sequence transformations for the acceleration of convergence
 // and the summation of divergent series. Computer Physics Reports, 10(5-6), 189-371.
 // Weniger, E.J. (1992). Interpolation between sequence transformations. Numerical Algorithms, 3(1-4), 477-486.

#include "series_acceleration.hpp"

namespace shanks{ namespace algos{

/**
 * @brief Weniger transformation class template implementing a Levin-type acceleration method.
 *
 * This transformation uses factorial series approximations for remainder estimates and is
 * particularly effective for alternating and factorially divergent series. It generalizes
 * Levin's transformation by using Pochhammer symbols instead of standard powers, providing
 * superior numerical stability and convergence properties in many cases.
 *
 * References:
 * - Weniger, E.J. (1989). Nonlinear sequence transformations for the acceleration of convergence
 *   and the summation of divergent series. Computer Physics Reports, 10(5-6), 189-371.
 * - Weniger, E.J. (1992). Interpolation between sequence transformations. Numerical Algorithms, 3(1-4), 477-486.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template<AcceptedLike T, UnsignedIntLike K>
class weniger_algorithm final : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Weniger Algorithm.
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 */
	explicit weniger_algorithm() : series_acceleration<T, K>("weniger") {}

	/**
	 * @brief Executes the Weniger transformation to accelerate series convergence.
	 *
	 * Computes the accelerated sum using a Levin-type transformation with factorial series
	 * weights. The algorithm relies on binomial coefficients and Pochhammer-like terms
	 * to estimate the remainder of the series and eliminate dominant error terms.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param n The number of terms to use in the transformation (starting index)
	 *        Valid values: n >= 0
	 * @param order The order of transformation (number of terms in binomial sum)
	 *        Valid values: order >= 1
	 * @param data series_result<T> struct containing necessary information for algorithm
	 * @return T The accelerated partial sum result.
	 * @throws std::out_of_range if the input vectors are smaller than required.
	 * @throws std::overflow_error if division by zero or numerical instability occurs.
	 */
	T operator()(
		const K n,
        const K order,
        const series_result<T>& data
	) const override;
};

template<AcceptedLike T, UnsignedIntLike K>
T weniger_algorithm<T, K>::operator()(
	const K n,
    const K order,
    const series_result<T>& data
) const {

    // Validation: ensure input vectors contain enough data for the specified order and base index
    const K required_size = n + order + static_cast<K>(1);
	const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));

    if (data.Sn.size() < required_size || data.an.size() < required_size) {
        throw std::out_of_range("The Sn or an smaller then required for Weniger_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + utils::to_string(required_size));
	}

    // Trivial case: order 0 returns the original partial sum
    if (order == static_cast<K>(0)) return data.Sn.at(n);

	// For theory, see: Weniger (1989), Section 8.2, Eq. (8.2-7)
	// Weniger transformation as ratio of binomial sums with Pochhammer symbols
	T numerator = utils::cast<T>(0.0, precision);
	T denominator = utils::cast<T>(0.0, precision);

	// For theory, see: Weniger (1989), Eq. (8.2-7) term components
	T rest;	// Weight factor for current term: (-1)ʲ × C(order, j) × (β+n+j)ₖ₋₁
					// Remainder estimate: 1/Δsₙ = 1/a_{n+1}

	// For theory, see: Weniger (1989), Eq. (8.2-7) weight factor
	// Initial Pochhammer-like term: (β+n)ₖ₋₁ with β=1, equivalent to (n+1)ₖ₋₁ = Γ(n+k)/Γ(n+1)
	T coef = utils::cast<T>(1.0, precision);

	// For theory, see: Weniger (1989), Eq. (8.2-7) recursive computation
	// Initial binomial coefficient: C(order, 0) = 1
	T binomial_coef = utils::cast<T>(utils::binomial_coefficient(n, static_cast<K>(0)), precision);

	// Precompute initial value: (1)ₖ₋₁ = (k-1)!
	for (K m = static_cast<K>(0); m < order - static_cast<K>(1); ++m) coef *= utils::cast<T>(static_cast<K>(1) + m);

	K j1;

	// For theory, see: Weniger (1989), Eq. (8.2-7) explicit summation form
	// Main summation loop for binomial transformation
	for (K j = static_cast<K>(0); j <= order; ++j) {

		j1 = j + static_cast<K>(1);

		// For theory, see: Weniger (1989), Eq. (8.2-7) term structure
		// Term sign: (-1)ʲ

		rest = utils::cast<T>(1.0, precision); //need to set precision before doing anything
		rest*= utils::minus_one_raised_to_power_n<T,K>(j);

		// Binomial coefficient: C(order, j)
		rest *= binomial_coef;

		// For theory, see: Weniger (1989), Eq. (8.2-7) recursive binomial update
		// Update binomial coefficient: C(order, j+1) = C(order, j) × (order - j) / (j + 1)
		binomial_coef *= utils::cast<T>(order - j, precision);
		binomial_coef /= utils::cast<T>(j1, precision);

		// For theory, see: Weniger (1989), Eq. (8.2-7) weight factor
		// Pochhammer symbol factor: (β+n+j)ₖ₋₁ with β=1
		rest *= coef;

		// For theory, see: Weniger (1989), Eq. (8.2-7) recursive Pochhammer update
		// Update Pochhammer-like term: (β+n+j+1)ₖ₋₁ = (β+n+j)ₖ₋₁ × (β+n+j+order) / (β+n+j+1)
		// With β=1: (n+j+1)ₖ₋₁ = (n+j)ₖ₋₁ × (n+j+order) / (n+j+1)
		coef *= utils::cast<T>(n + j + order, precision);
		coef /= utils::cast<T>(n + j1, precision);

		// For theory, see: Weniger (1989), Eq. (8.2-7) remainder estimate
		// Remainder estimate: ωₙ = Δsₙ = a_{n+1}, so 1/ωₙ = 1/a_{j+1}
		rest /= data.an.at(n + j1);

		// For theory, see: Weniger (1989), Eq. (8.2-7) numerator term
		// Numerator term: rest × s_{n+j}
		numerator   += rest * data.Sn.at(n + j);

		// For theory, see: Weniger (1989), Eq. (8.2-7) denominator term
		// Denominator term: rest
		denominator += rest;

	}

	// For theory, see: Weniger (1989), Eq. (8.2-7) final ratio
	// Final transformed value: δₖ⁽ⁿ⁾ = numerator / denominator
	numerator /= denominator;

	// Check if the result is valid
	if(!utils::isfinite(numerator)) throw std::overflow_error("division by zero");

	return numerator;
}

} //namespace shanks::algos
} //namespace shanks

#endif
