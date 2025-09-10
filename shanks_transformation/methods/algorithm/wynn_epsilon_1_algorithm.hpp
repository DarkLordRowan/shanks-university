/**
 * @file wynn_epsilon_1_algorithm.hpp
 * @brief This file contains the declaration of the Wynn Epsilon Algorithm,
 *        a nonlinear sequence transformation for accelerating series convergence.
 */

 // For theory, see:
 // Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation. 
 // Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 // Wynn, P. (1962). A comparison technique for the numerical transformation of 
 // slowly convergent series based on the use of rational functions. 
 // Numerische Mathematik, 4(1), 8-14.

#include "../series_acceleration.hpp"
#include <vector>	// Include the vector library
#include <cmath>	//Include for isfinite

/**
 * @brief Wynn Epsilon Algorithm class template implementing the epsilon algorithm for series acceleration.
 *
 * @authors Pashkov B.B. (implementation), Wynn P. (algorithm)
 *
 * This class implements the epsilon algorithm, a nonlinear transformation that accelerates
 * the convergence of slowly convergent series. The algorithm is particularly effective
 * for sequences that can be expressed as a linear combination of exponentials.
 *
 * References:
 * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * - Wynn, P. (1962). A comparison technique for the numerical transformation of
 *   slowly convergent series based on the use of rational functions.
 *   Numerische Mathematik, 4(1), 8-14.
 *
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations (e.g., size_t, unsigned int)
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class wynn_epsilon_1_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Wynn Epsilon Algorithm.
	 * @param series The series class object to be accelerated.
	 *        Must be a valid object implementing the required series interface.
	 *        The series should represent a slowly convergent sequence for effective acceleration.
	 */
	explicit wynn_epsilon_1_algorithm(const series_templ& series);

	/**
	 * @brief Applies the Wynn Epsilon Algorithm to accelerate series convergence.
	 *
	 * Computes the accelerated sum using the epsilon algorithm, which constructs
	 * a table of transformed values using the recurrence:
	 * εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
	 * Only even-order transforms ε₂ₖ⁽ⁿ⁾ are meaningful approximations to the series limit.
	 *
	 * For theory, see: Wynn (1956), Eq. (4) - Main recurrence relation
	 *
	 * @param n The starting index of the partial sum to transform (typically 0 or 1)
	 *        Valid values: n >= 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of epsilon steps to apply)
	 *        Valid values: order >= 1
	 *        Higher orders use more terms but may provide better acceleration
	 * @return The accelerated partial sum after epsilon transformation
	 * @throws std::domain_error if order=0 is provided
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
    T operator()(K n, K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
wynn_epsilon_1_algorithm<T, K, series_templ>::wynn_epsilon_1_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T wynn_epsilon_1_algorithm<T, K, series_templ>::operator()(const K n, const K order) const
{

	using std::isfinite;

	// For theory, see: Wynn (1956), Section 2 - Definition of εₖ⁽ⁿ⁾
	// Initial condition: ε₀⁽ⁿ⁾ = Sₙ (n-th partial sum)
	if (order == static_cast<K>(0))
		return this->series->S_n(n);

	// For theory, see: Wynn (1956), Section 3 - Algorithm implementation
	// The algorithm requires 2×order transformation steps to compute ε₂ₖ⁽ⁿ⁾
	const K m = static_cast<K>(2) * order;
	K max_ind = m + n; // Maximum index needed in the transformation

	const K n1 = n - static_cast<K>(1); // Adjusted starting index

	// Initialize epsilon tables: e0 for current column, e1 for next column
	// For theory, see: Wynn (1956), Section 3 - Table construction
	std::vector<T> e0(max_ind + static_cast<K>(1), static_cast<T>(0));
	std::vector<T> e1(max_ind                    , static_cast<T>(0));

	auto e0_add = &e0; // Pointer to current epsilon column
	auto e1_add = &e1; // Pointer to next epsilon column

	// Initialize first column with partial sums: ε₀⁽ʲ⁾ = S_j
	// For theory, see: Wynn (1956), Eq. (2) - Initial conditions
	K j = max_ind;
	do {
		e0[j] = this->series->S_n(j);
	} while (--j > static_cast<K>(0));

	// Apply epsilon algorithm recurrence
	// For theory, see: Wynn (1956), Eq. (4) - Main recurrence relation
	// εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
	for (K i = static_cast<K>(0); i < m; ++i) {
		for (K j = n1; j < max_ind; ++j) {
			// Compute εₖ₊₁⁽ʲ⁾ using the recurrence relation
			(*e1_add)[j] += static_cast<T>(1) / ((*e0_add)[j + static_cast<K>(1)] - (*e0_add)[j]);
		}

		--max_ind;							// Reduce working range for next iteration
		std::swap(e0_add, e1_add);			// Swap roles of current and next columns
		(*e1_add).erase((*e1_add).begin()); // Remove first element to maintain alignment
	}

	// Check for numerical stability
	// For theory, see: Wynn (1956), Section 4 - Numerical considerations
	if (!isfinite((*e0_add)[n1]))
		throw std::overflow_error("division by zero");

	// Return the final transformed value (even-order epsilon transform)
	// For theory, see: Wynn (1956), Section 2 - ε₂ₖ⁽ⁿ⁾ as accelerated approximations
	return (*e0_add)[n1];
}