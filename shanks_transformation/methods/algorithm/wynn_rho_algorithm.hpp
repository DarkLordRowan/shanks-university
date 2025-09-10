/**
 * @file wynn_rho_algorithm.hpp
 * @brief This file contains the declaration of the Wynn's Rho Algorithm class.
 * @authors Yurov P.I. Bezzaborov A.A.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
 * Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
 */

#pragma once

#include <memory> // For std::unique_ptr

#include "../series_acceleration.hpp"
#include "../wynn_numerators.hpp"

 /**
  * @brief Wynn's Rho Algorithm class template for sequence acceleration.
  *
  * This class implements Wynn's rho algorithm, which is particularly effective
  * for accelerating logarithmically convergent sequences. The algorithm uses
  * continued fraction representations and reciprocal differences for convergence
  * acceleration.
  *
  * References:
  * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
  * - Wynn, P. (1966). Upon a second-order continued-fraction transformation for accelerating convergence.
  * - Osada, N. (1993). Acceleration Methods for Slowly Convergent Sequences and Their Applications.
  *
  * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
  *           Represents numerical precision (float, double, long double)
  *           Used for all mathematical computations and storage
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting terms, indexing operations, and order specification
  *           Valid values: K >= 0, typically size_t or unsigned int
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term a_n
  *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
  */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class wynn_rho_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

	std::unique_ptr<const numerator_base<T, K>> numerator;	/**< Numerator computation strategy */
	const T gamma;											/**< Gamma parameter for generalized rho transformation */
	const T RHO;											/**< Rho parameter for gamma-rho variant */

	/**
	 * @brief Main calculation method for Wynn's rho algorithm.
	 * @param n Starting index for partial sums
	 * @param order Transformation order (must be even)
	 * @return Accelerated partial sum
	 */
	inline T calculate(K n, K order) const;

	/**
	 * @brief Recursive computation body for the rho algorithm.
	 *
	 * For theory, see: Wynn (1966), Eq. (3.2) - Recursive formulation
	 * ρₖ⁽ⁿ⁾ = ρₖ₋₂⁽ⁿ⁺¹⁾ + [γ + k(RHO - 1)] / [ρₖ₋₁⁽ⁿ⁺¹⁾ - ρₖ₋₁⁽ⁿ⁾]
	 *
	 * @param n Current index
	 * @param order Current recursion order
	 * @param S_n Current partial sum value
	 * @param j Adjustment index for term selection
	 * @return Computed transformation value
	 */
	T recursive_calculate_body(K n, K order, T S_n, K j) const;

public:

	/**
	 * @brief Parameterized constructor to initialize the Rho Wynn Algorithm.
	 *
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 * @param variant Type of numerator computation strategy
	 *        Determines the specific variant of Wynn's rho algorithm:
	 *        - rho_variant: Standard Wynn rho algorithm
	 *        - generalized_variant: Generalized rho transformation
	 *        - gamma_rho_variant: Gamma-rho variant with additional parameters
	 * @param gamma_ Gamma parameter value (default: 1)
	 *        Valid values: T > 0, typically 1 or 2
	 *        Controls the transformation behavior in generalized variants
	 * @param RHO_ Rho parameter value (default: 0)
	 *        Valid values: T >= 0, typically 0 or 1
	 *        Additional parameter for gamma-rho variant
	 */
	explicit wynn_rho_algorithm(
		const series_templ& series,
		numerator_type variant = numerator_type::rho_variant,
		T gamma_ = T(1),
		T RHO_ = T(0)
	);

	//Default destructor is sufficient since unique_ptr handles deletion

	/**
	 * @brief Wynn's rho algorithm transformation.
	 *
	 * Computes the accelerated partial sum using Wynn's rho algorithm.
	 * The algorithm is particularly effective for logarithmically convergent
	 * sequences and uses continued fraction representations for acceleration.
	 *
	 * For theory, see: Wynn (1966), Main Theorem
	 * ρ₂ₖ⁽ⁿ⁾ provides accelerated convergence for sequences satisfying
	 * S_n ~ S + Σ c_k n^{-k} as n → ∞
	 *
	 * @param n The starting index for partial sums
	 *        Valid values: n >= 0, determines the starting point of transformation
	 * @param order The order of transformation
	 *        Valid values: order >= 0 and must be even (order % 2 == 0)
	 *        Higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after rho transformation
	 * @throws std::domain_error if order is odd
	 * @throws std::overflow_error if division by zero occurs
	 */
	T operator()(const K n, const K order) const override { return calculate(n, order); }

	/**
	 * @brief Compute transformed partial sum (extended version for arbitrary precision)
	 *
	 * Template version for compatibility with larger integer types while
	 * maintaining the core algorithm implementation.
	 *
	 * @tparam BigK Type for term count (must be convertible to K)
	 * @tparam BigOrder Type for order (must be convertible to int)
	 * @param n Number of terms (passed by reference for large types)
	 * @param order Transformation order (must be even)
	 * @return The accelerated partial sum
	 * @throws std::domain_error for invalid arguments
	 * @throws std::overflow_error for division by zero
	 */
	template <typename BigK, typename BigOrder, typename = std::enable_if_t<!std::is_same_v<BigK, K> || !std::is_same_v<BigOrder, K>>> T operator()(const BigK& n, const BigOrder& order) const {
		static_assert(std::is_constructible_v<K, BigK>, "Term count type must be convertible to K");

		return calculate_impl(static_cast<K>(n), static_cast<int>(order));
	}
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
wynn_rho_algorithm<T, K, series_templ>::wynn_rho_algorithm(
	const series_templ& series,
	const numerator_type variant,
	const T gamma_,
	const T RHO_
	) :
	series_acceleration<T, K, series_templ>(series),
	gamma(gamma_),
	RHO(RHO_)
{
	//TODO: проверить можно ли использовать стандартные значения, указанные в конструктуре, в других вариантах + унифицировать названия
	// есть ли какие то ограничения на кастомные значения?
	// classic (0), gamma (1), gamma-rho (2):
	// 0 -> gamma = 1, RHO = 0
	// 1 -> gamma = 2, RHO = 0
	// 2 -> gamma = 2, RHO = 1

	// Initialize the appropriate numerator computation strategy
	switch(variant) {
		case numerator_type::rho_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		case numerator_type::generalized_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		case numerator_type::gamma_rho_variant :
			numerator.reset(new rho_transform<T, K>());
			break;
		default:
			numerator.reset(new rho_transform<T, K>());
	}
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
inline T wynn_rho_algorithm<T, K, series_templ>::calculate(const K n, K order) const { //const int order

	using std::isfinite;

	// For theory, see: Wynn (1966), Theorem 1 - Order must be even
	if (order & static_cast<K>(1))// is order odd
		//++order; //why
		throw std::domain_error("order should be even number");

	if (order == static_cast<K>(0)) return this->series->S_n(n);

	const T S_n = this->series->S_n(n);
	const K order1 = order - static_cast<K>(1);

	// For theory, see: Wynn (1966), Eq. (3.5) - Main recurrence relation
	// ρₖ⁽ⁿ⁾ = ρₖ₋₂⁽ⁿ⁺¹⁾ + Nₖ⁽ⁿ⁾ / (ρₖ₋₁⁽ⁿ⁺¹⁾ - ρₖ₋₁⁽ⁿ⁾)
	const T res = 
		recursive_calculate_body(
			n,
			order1 - static_cast<K>(1),
			 S_n,
			 static_cast<K>(1)
		) +
		numerator->operator()(
			n,
			order,
			this->series,
			gamma,
			RHO
		) / (
		recursive_calculate_body(
			n,
			order1,
			S_n,
			static_cast<K>(1)
		) -
		recursive_calculate_body(
			n,
			order1,
			S_n,
			static_cast<K>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");
	return res;
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T wynn_rho_algorithm<T, K, series_templ>::recursive_calculate_body(const K n, const K order, T S_n, const K j) const {

	using std::isfinite;
	/**
	* S_n - previous sum;
	* j - adjusts n: (n + j);
	*/

	// For theory, see: Wynn (1966), Eq. (3.3) - Base case initialization
	// Add current term if j > 0 (j=0 means use current S_n without adding new terms)
	S_n += (j == static_cast<K>(0)) ? static_cast<T>(0) : this->series->operator()(n + j);

	// Base case: return current partial sum when order reaches 0
	if (order == static_cast<K>(0))
		return S_n;

	// Invalid order case
	if (order == static_cast<K>(-1))
		return static_cast<T>(0);

	const K order1 = order - static_cast<K>(1);
	const K nj = n + j;

	// For theory, see: Wynn (1966), Eq. (3.5) - Recursive computation
	// ρₖ⁽ⁿ⁾ = ρₖ₋₂⁽ⁿ⁺¹⁾ + Nₖ⁽ⁿ⁾ / (ρₖ₋₁⁽ⁿ⁺¹⁾ - ρₖ₋₁⁽ⁿ⁾)
	const T res = 
		recursive_calculate_body(
			nj,
			order1 - static_cast<K>(1),
			S_n,
			static_cast<K>(1)
		) +
		numerator->operator()(
			nj,
			order,
			this->series,
			gamma,
			RHO
		) / (
		recursive_calculate_body(
			nj,
			order1,
			 S_n,
			 static_cast<K>(1)
		) -
		recursive_calculate_body(
			nj,
			order1,
			S_n,
			static_cast<K>(0)
		)
	);

	if (!isfinite(res))
		throw std::overflow_error("division by zero");

	return res;
}
