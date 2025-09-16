/**
 * @file wynn_epsilon_2_algorithm.hpp
 * @brief This file contains the declaration of the second implementation of Wynn's Epsilon Algorithm.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * Wynn, P. (1964). General Purpose Vector Epsilon Algorithm ALGOL Procedures.
 *   Numerische Mathematik, 6, 22-36.
 * Wynn, P. (1971). A Note on the Generalised Euler Transformation.
 *   Computer Journal, 14(4), 437-441.
 */

#pragma once

#include "../series_acceleration.hpp"
#include <vector>   // Include the vector library for std::vector
#include <cmath>    // Include for std::isfinite

 /**
  * @brief Wynn's Epsilon Algorithm (Second Implementation) class template.
  *
  * This class implements Wynn's epsilon algorithm for series acceleration.
  * The algorithm constructs a table of approximations using a recurrence relation
  * and returns accelerated partial sums from the even columns of this table.
  *
  * Template Parameters:
  * @tparam T Floating-point type for series elements (must satisfy Accepted)
  *           Represents numerical precision (float, double, long double)
  * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
  *           Used for counting and indexing operations (typically std::size_t)
  * @tparam series_templ Type of series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term aₙ
  *           - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
  *
  * Mathematical Formulation:
  * Initial conditions: ε₋₁⁽ᵐ⁾ = 0, ε₀⁽ᵐ⁾ = Sₙ for m = 0,1,2,...
  * Recurrence relation: εₖ₊₁⁽ᵐ⁾ = εₖ₋₁⁽ᵐ⁺¹⁾ + 1/(εₖ⁽ᵐ⁺¹⁾ - εₖ⁽ᵐ⁾) for k, m = 0,1,2,...
  * Accelerated approximations are found in the even columns: ε₂ₖ⁽ᵐ⁾
  *
  * References:
  * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
  * - Wynn, P. (1964). General Purpose Vector Epsilon Algorithm ALGOL Procedures.
  */
template <Accepted T, std::unsigned_integral K, typename series_templ>
class wynn_epsilon_2_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Epsilon Algorithm.
	 * @param series The series class object to be accelerated.
	 *        Must be a valid object implementing the required series interface.
	 *        The series should provide term access and partial sum calculation.
	 */
    explicit wynn_epsilon_2_algorithm(const series_templ& series);

	/**
	 * @brief Implementation of Wynn's epsilon algorithm for series acceleration.
	 *
	 * Computes the accelerated sum using Wynn's epsilon algorithm with improved
	 * numerical stability checks. The algorithm constructs a table of approximations
	 * and returns the most accurate estimate from the even columns.
	 *
	 * Mathematical Formulation:
	 * For theory, see: Wynn (1956), Eq. (4) - Epsilon algorithm recurrence relation ()
	 * More information, see page 20 - 21 in[https://hal.science/hal-04207550/document]
	 * 
	 * εₖ₊₁⁽ᵐ⁾ = εₖ₋₁⁽ᵐ⁺¹⁾ + 1/(εₖ⁽ᵐ⁺¹⁾ - εₖ⁽ᵐ⁾)
	 *
	 * @param n The number of terms to use in the transformation (n ≥ 1)
	 *        Valid values: n > 0 (algorithm requires at least 1 term)
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (typically order ≤ n/2)
	 *        Valid values: order >= 0
	 *        Higher orders use more terms from the epsilon table
	 * @return The accelerated partial sum after Wynn's epsilon transformation
	 * @throws std::domain_error if n=0 is provided as input
	 * @throws std::overflow_error if numerical instability occurs
	 */
    T operator()(K n, K order) const override;
};

template <Accepted T, std::unsigned_integral K, typename series_templ>
wynn_epsilon_2_algorithm<T, K, series_templ>::wynn_epsilon_2_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <Accepted T, std::unsigned_integral K, typename series_templ>
T wynn_epsilon_2_algorithm<T, K, series_templ>::operator()(const K n, const K order) const
{

	using std::isfinite;

	// For theory, see: Wynn (1956), Section 2 - Initial conditions and algorithm setup
	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");
	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	// For theory, see: Wynn (1956), Section 3 - Algorithm construction and table size
	// Total number of entries needed in the epsilon table: k = 2*order + n
	K k = static_cast<K>(2) * order + n;

	// For theory, see: Wynn (1956), Section 3 - Epsilon table structure
	// The epsilon table is stored as a 4-row circular buffer to save memory
	std::vector<std::vector<T>> eps(
		4,
		std::vector<T>(
			k + static_cast<K>(1),
			static_cast<T>(0)
		)
	);

	// For theory, see: Wynn (1956), Eq. (2) - Initialization with partial sums
	// Initialize the bottom row with partial sums: ε₀⁽ᵐ⁾ = Sₙ for m = 0,1,...,k
	for (K i = static_cast<K>(0); i <= k; ++i)
		eps[3][i] = this->series->S_n(i);


	T a, a1, a2;
	a = a1 = a2 = static_cast<T>(0);

	K i1, i2;

	// For theory, see: Wynn (1956), Section 3 - Lozenge computation process
    // Build the epsilon table from bottom to top using the recurrence relation
	while (k > static_cast<K>(0))
	{
		for (K i = static_cast<K>(0); i != k; ++i)
		{
			i1 = i + static_cast<K>(1);
			i2 = i + static_cast<K>(2);

			// For theory, see: Wynn (1956), Eq. (4) - Main recurrence relation
			// εₖ₊₁⁽ᵐ⁾ = εₖ₋₁⁽ᵐ⁺¹⁾ + 1/(εₖ⁽ᵐ⁺¹⁾ - εₖ⁽ᵐ⁾)
			eps[0][i] = eps[2][i1] + static_cast<T>(1) / (eps[3][i1] - eps[3][i]);
			
			// For theory, see: Wynn (1964) - Numerical stability improvements
		    // Additional checks and corrections for finite precision arithmetic
			if (!isfinite(eps[0][i]) && i2 <= k) // Stability check and correction
			{
				a2 = static_cast<T>(1) / eps[2][i1];

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[2][i2]));
				a = eps[2][i2] * a1;

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[2][i]));
				a += eps[2][i] * a1;

				a1 = static_cast<T>(1) / (static_cast<T>(1) - (a2 * eps[0][i2]));
				a -= eps[0][i2] * a1;

				eps[0][i] = static_cast<T>(1) / eps[2][i1];
				eps[0][i] = static_cast<T>(1) / (static_cast<T>(1) + a * eps[0][i]);
				eps[0][i] = eps[0][i] * a;
			}

			// Fallback to previous value if correction fails
			if (!isfinite(eps[0][i]))
				eps[0][i] = eps[2][i];

		}

		// For theory, see: Wynn (1956), Section 3 - Table updating procedure
		// Shift rows upward in the circular buffer for the next iteration
		std::swap(eps[0], eps[1]);
		std::swap(eps[1], eps[2]);
		std::swap(eps[2], eps[3]);

		--k;
	}

	// Final row shifts to position the result correctly
	std::swap(eps[0], eps[1]);
	std::swap(eps[1], eps[2]);
	std::swap(eps[2], eps[3]);

	// For theory, see: Wynn (1956), Section 3 - Result extraction
	// Even columns (ε₂ₖ⁽ᵐ⁾) contain the accelerated approximations
	if (n % static_cast<K>(2) != static_cast<K>(0))
		return eps[3][0];

	return eps[0][0];

}
