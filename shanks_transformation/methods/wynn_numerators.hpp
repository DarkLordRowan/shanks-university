/**
 * @file wynn_numerators.h
 * @brief This file contains various variants of numerators for Wynn-type transformations.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * Wynn, P. (1962). A comparison technique for the numerical transformation of slowly convergent series based on the use of rational functions.
 *   Numerische Mathematik, 4(1), 8-14.
 * Wynn, P. (1964). General Purpose Vector Epsilon Algorithm ALGOL Procedures.
 *   Numerische Mathematik, 6(1), 22-36.
 *
 * @authors Yurov P.I., Bezzaborov A.A.
 */

#pragma once

#include "../series.h"

 /**
  * @brief Enum for remainder types to use in Levin-type transformations.
  *
  * Determines the type of numerator function used in the transformation:
  * - rho_variant: Standard ρ-variant (difference-based)
  * - generalized_variant: Generalized variant (order-dependent constant)
  * - gamma_rho_variant: Gamma-ρ variant (parameter-dependent)
  */
enum numerator_type{
           rho_variant,
   generalized_variant,
     gamma_rho_variant,
};

/**
 * @brief Abstract base class for numerator functions in Wynn-type transformations.
 *
 * Defines the interface for numerator computation used in series acceleration methods.
 *
 * Template Parameters:
 * @tparam T Floating-point type for series elements and computations.
 *           Must satisfy std::floating_point. Represents numerical precision.
 * @tparam K Unsigned integral type for indices and counts.
 *           Must satisfy std::unsigned_integral. Used for indexing and order specification.
 */
template<std::floating_point T, std::unsigned_integral K>
class numerator_base {
public:

	/**
	 * @brief Compute the numerator value for Wynn-type transformation.
	 *
	 * Pure virtual function to be implemented by specific numerator variants.
	 *
	 * @param n Index of the starting term in the series.
	 *        Valid values: n >= 0. Determines the position in the series.
	 * @param order Order of the transformation (number of terms used).
	 *        Valid values: order >= 0. Higher orders use more terms.
	 * @param series Pointer to the series object providing term access.
	 *        Must be non-null and valid throughout the computation.
	 * @param gamma Gamma parameter for parameterized variants.
	 *        Meaning depends on the specific variant. Default: 1.0.
	 * @param rho Rho parameter for parameterized variants.
	 *        Meaning depends on the specific variant. Default: 0.0.
	 * @return The computed numerator value for the transformation.
	 */
	virtual T operator()(K n, K order, const series_base<T, K>* series, T gamma = T(1), T rho = T(0)) const = 0;

};

/**
 * @brief Rho-variant numerator class (difference-based).
 *
 * Implements the standard numerator based on term differences:
 * Numerator = a_{n+order} - a_n
 *
 * For theory, see: Wynn (1956), Eq. (2.6b) and related difference formulations.
 *
 * Template Parameters:
 * @tparam T Floating-point type for series elements.
 * @tparam K Unsigned integral type for indices.
 */
template<std::floating_point T, std::unsigned_integral K>
class rho_transform : public numerator_base<T, K> {
public:

	/**
	 * @brief Compute the rho-variant numerator.
	 *
	 * Implements: numerator = term(n + order) - term(n)
	 * This represents the forward difference of order 'order' at index n.
	 *
	 * @param n Starting index in the series.
	 *        Valid values: n >= 0, n + order within series bounds.
	 * @param order Order of the difference.
	 *        Valid values: order >= 0.
	 * @param series Pointer to the series object for term access.
	 *        Must provide operator()(K) for term retrieval.
	 * @param gamma Unused parameter (maintained for interface consistency).
	 * @param rho Unused parameter (maintained for interface consistency).
	 * @return The computed difference: series(n+order) - series(n).
	 */
	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {

		// For theory, see: Wynn (1956), Eq. (2.6b): ΔS_n = S_{n+1} - S_n
		return (series->operator()(n + order) - series->operator()(n));
	}
};

/**
 * @brief Generalized-variant numerator class (order-dependent constant).
 *
 * Implements a numerator that is a function of the transformation order:
 * Numerator = order - gamma - 1
 *
 * This variant provides a constant numerator based on the order and gamma parameter.
 *
 * Template Parameters:
 * @tparam T Floating-point type for series elements.
 * @tparam K Unsigned integral type for indices.
 */
template<typename T, typename K>
class generilized_transform : public numerator_base<T, K> {
public:

	/**
	 * @brief Compute the generalized-variant numerator.
	 *
	 * Implements: numerator = order - gamma - 1
	 * This provides a order-dependent constant numerator.
	 *
	 * @param n Unused parameter (maintained for interface consistency).
	 * @param order Order of the transformation.
	 *        Valid values: order >= 0.
	 * @param series Unused parameter (maintained for interface consistency).
	 * @param gamma Gamma parameter offset.
	 *        Valid values: any finite T value. Typical range: 0.0 to 2.0.
	 * @param rho Unused parameter (maintained for interface consistency).
	 * @return The computed constant: order - gamma - 1.
	 */
	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {
		return static_cast<T>(order) - gamma - static_cast<T>(1);
	}
};

/**
 * @brief Gamma-Rho-variant numerator class (parameter-dependent).
 *
 * Implements a numerator that depends on both order and parameters gamma and rho:
 * For even orders: numerator = -gamma + (order/2)/rho
 * For odd orders:  numerator = -gamma + (order/2)/rho + 1
 *
 * This variant provides a flexible parameterized numerator for specialized transformations.
 *
 * Template Parameters:
 * @tparam T Floating-point type for series elements.
 * @tparam K Unsigned integral type for indices.
 */
template<std::floating_point T, std::unsigned_integral K>
class gamma_rho_transform : public numerator_base<T, K> {
public:

	/**
	 * @brief Compute the gamma-rho-variant numerator.
	 *
	 * Implements a piecewise definition based on order parity:
	 * If order is even: numerator = -gamma + (order/2)/rho
	 * If order is odd:  numerator = -gamma + (order/2)/rho + 1
	 *
	 * @param n Unused parameter (maintained for interface consistency).
	 * @param order Order of the transformation.
	 *        Valid values: order >= 0.
	 * @param series Unused parameter (maintained for interface consistency).
	 * @param gamma Gamma parameter (additive offset).
	 *        Valid values: any finite T value. Typical range: -10.0 to 10.0.
	 * @param rho Rho parameter (divisor scaling).
	 *        Valid values: rho ≠ 0.0 to avoid division by zero.
	 * @return The computed parameter-dependent numerator.
	 * @throws std::invalid_argument if rho = 0.0.
	 */
	T operator()(const K n, const K order, const series_base<T, K>* series, const T gamma = static_cast<T>(1), const T rho = static_cast<T>(0)) const {

		// insight: order % 2 is the same order & 1
		// if order is even:
		// order % 2 = 0
		// order & 1 = 0
		// else order is odd:
		// order % 2 = 1
		// order & 1 = 1


		// For theory, see: Wynn (1962), Section 2: Parameterized transformations
		T base = -gamma + static_cast<T>(order / static_cast<K>(2)) / rho;

		// Add 1 for odd orders (order & 1 checks parity)
		return base + static_cast<T>(order & static_cast<K>(1));
	}
};