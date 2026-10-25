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
    if (data.Sn.size() < required_size) {
        throw std::out_of_range("Insufficient partial sums for Wynn epsilon");
    }
    if (order == static_cast<K>(0)) return data.Sn.at(n);
    const std::size_t precision = utils::helpers<T>::get_precision(data.Sn[0]);
    const std::size_t count = required_size - static_cast<std::size_t>(n);
    // Wynn recurrence, https://dlmf.nist.gov/3.9#E11.
    // previous is epsilon_-1, current is epsilon_0. Keep the n+1
    // alignment explicitly; n is a zero-based index, including n=0.
    std::vector<T> previous(count + 1, utils::cast<T, int>()(0, precision));
    std::vector<T> current(data.Sn.begin() + n, data.Sn.begin() + required_size);
    for (std::size_t column = 1; column < count; ++column) {
        std::vector<T> next(current.size() - 1);
        for (std::size_t j = 0; j < next.size(); ++j) {
            const T difference = current[j + 1] - current[j];
            if (difference == utils::cast<T, int>()(0, precision))
                throw std::overflow_error("Singular Wynn epsilon table");
            next[j] = previous[j + 1] + utils::cast<T, int>()(1, precision) / difference;
            if (!utils::helpers<T>::isfinite(next[j]))
                throw std::overflow_error("Non-finite Wynn epsilon cell");
        }
        previous = std::move(current);
        current = std::move(next);
    }
    return current[0];
}

}  // namespace algos
}  // namespace shanks

#endif
