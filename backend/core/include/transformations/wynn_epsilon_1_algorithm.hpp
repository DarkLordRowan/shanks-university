#ifndef WYNN_EPSILON_1_ALGORITHM_HPP
#define WYNN_EPSILON_1_ALGORITHM_HPP
#pragma once
/**
 * @file wynn_epsilon_1_algorithm.hpp
 * @brief This file contains the declaration of the Wynn Epsilon Algorithm,
 *        a nonlinear sequence transformation for accelerating series convergence.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

// For theory, see:
// Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
// Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
// Wynn, P. (1962). A comparison technique for the numerical transformation of
// slowly convergent series based on the use of rational functions.
// Numerische Mathematik, 4(1), 8-14.

namespace shanks {
namespace algos {

/**
 * @brief Wynn Epsilon Algorithm class template implementing the epsilon algorithm for series acceleration.
 *
 * This class implements the epsilon algorithm, a nonlinear transformation that accelerates
 * the convergence of slowly convergent series. It is particularly effective for sequences
 * that can be expressed as a linear combination of exponentials. The algorithm builds a
 * triangular table of estimates using a simple recursive rule.
 * References:
 * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * - Wynn, P. (1962). A comparison technique for the numerical transformation of
 *   slowly convergent series based on the use of rational functions.
 *   Numerische Mathematik, 4(1), 8-14.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations (e.g., size_t, unsigned int)
 */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_1_algorithm final : public series_acceleration<T, K> {
public:
    /**
     * @brief Parameterized constructor to initialize the Wynn Epsilon Algorithm.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit wynn_epsilon_1_algorithm() : series_acceleration<T, K>("wynn epsilon 1") {};

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
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.

     * @param n The starting index of the partial sum to transform (typically 0 or 1)
     *        Valid values: n >= 0
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (number of epsilon steps to apply)
     *        Valid values: order >= 1
     *        Higher orders use more terms but may provide better acceleration
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return T The accelerated partial sum result.
     * @throws std::out_of_range if the Sn vector size is insufficient for index n and order.
     * @throws std::domain_error if n is 0.
     * @throws std::overflow_error if division by zero or numerical instability occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        return n + std::size_t(2) * order + std::size_t(1);
    }
};

template <AcceptedLike T, UnsignedIntLike K>
T wynn_epsilon_1_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Ensure we have enough data points: 2*order + n + 1 terms are required
    const std::size_t required_size = wynn_epsilon_1_algorithm<T, K>::how_much(n, order);
    const std::size_t precision = utils::helpers<T>::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_1_{" +
                                utils::helpers<K>::to_string(order) + "}^{" + utils::helpers<K>::to_string(n) + "}\n" +
                                "the size of Sn must be at least " +
                                utils::helpers<std::size_t>::to_string(required_size));
    }

    if (n == static_cast<K>(0)) throw std::domain_error("n = 0 in the input");
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // For theory, see: Wynn (1956), Section 3 - Algorithm implementation
    // The algorithm requires 2×order transformation steps to compute ε₂ₖ⁽ⁿ⁾
    const K m = static_cast<K>(2) * order;
    K max_ind = m + n;  // Maximum index needed in the transformation

    const K n1 = n - static_cast<K>(1);  // Adjusted starting index

    // Initialize epsilon tables: e0 for current column, e1 for next column
    // For theory, see: Wynn (1956), Section 3 - Table construction
    std::vector<T> e0(max_ind + static_cast<K>(1), utils::cast<T, int>()(0, precision));
    std::vector<T> e1(max_ind, utils::cast<T, int>()(0, precision));

    auto e0_add = &e0;  // Pointer to current epsilon column
    auto e1_add = &e1;  // Pointer to next epsilon column

    // Initialize first column with partial sums: ε₀⁽ʲ⁾ = S_j
    // For theory, see: Wynn (1956), Eq. (2) - Initial conditions
    K j = max_ind;
    do {
        e0[j] += data.Sn.at(j);
    } while (--j > static_cast<K>(0));

    // Apply epsilon algorithm recurrence
    // For theory, see: Wynn (1956), Eq. (4) - Main recurrence relation
    // εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
    for (K i = static_cast<K>(0); i < m; ++i) {
        for (K j = n1; j < max_ind; ++j) {
            // Compute εₖ₊₁⁽ʲ⁾ using the recurrence relation
            (*e1_add)[j] += utils::cast<T, int>()(1, precision) / ((*e0_add)[j + static_cast<K>(1)] - (*e0_add)[j]);
        }

        --max_ind;                           // Reduce working range for next iteration
        std::swap(e0_add, e1_add);           // Swap roles of current and next columns
        (*e1_add).erase((*e1_add).begin());  // Remove first element to maintain alignment
    }

    // Check for numerical stability
    // For theory, see: Wynn (1956), Section 4 - Numerical considerations
    if (!utils::helpers<T>::isfinite((*e0_add)[n1])) throw std::overflow_error("division by zero");

    // Return the final transformed value (even-order epsilon transform)
    // For theory, see: Wynn (1956), Section 2 - ε₂ₖ⁽ⁿ⁾ as accelerated approximations
    return (*e0_add)[n1];
}

}  // namespace algos
}  // namespace shanks

#endif
