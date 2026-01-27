#ifndef WYNN_EPSILON_2_ALGORITHM_HPP
#define WYNN_EPSILON_2_ALGORITHM_HPP
#pragma once

/**
 * @file wynn_epsilon_2_algorithm.hpp
 * @brief This file contains the declaration of the second implementation of Wynn's Epsilon Algorithm.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * Wynn, P. (1964). General Purpose Vector Epsilon Algorithm ALGOL Procedures.
 *   Numerische Mathematik, 6, 22-36.
 * Wynn, P. (1971). A Note on the Generalised Euler Transformation.
 *   Computer Journal, 14(4), 437-441.
 */

#include "series_acceleration.hpp"

namespace shanks {
namespace algos {

/**
 * @brief Wynn's Epsilon Algorithm (Second Implementation) class template.
 *
 * This class implements Wynn's epsilon algorithm for series acceleration using a circular
 * buffer of rows to minimize memory usage. It includes stability checks and correction
 * logic to handle potential numerical singular points where the recurrence might fail.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * Template Parameters:
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations (typically std::size_t)
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
template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_2_algorithm final : public series_acceleration<T, K> {
public:
    /**
     * @brief Parameterized constructor to initialize the Epsilon Algorithm.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit wynn_epsilon_2_algorithm() : series_acceleration<T, K>("wynn epsilon 2") {};

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
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n The number of terms to use in the transformation (n ≥ 1)
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (typically order ≤ n/2)
     *        Valid values: order >= 0
     *        Higher orders use more terms from the epsilon table
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return T The accelerated partial sum result.
     * @throws std::out_of_range if the Sn vector is too small for the requested order.
     * @throws std::domain_error if n is 0.
     * @throws std::overflow_error if a non-finite result is encountered despite stability checks.
     */
    T operator()(const K n, const K order,

                 const series_result<T>& data) const override;
};

template <AcceptedLike T, UnsignedIntLike K>
T wynn_epsilon_2_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Ensure sufficient terms are available: 2*order + n + 1 terms are required
    const K required_size = static_cast<K>(2) * order + n + static_cast<K>(1);
    const size_t precision = utils::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range("The Sn vector is smaller than required for Wynn epsilon 2 computation.");
    }

    // For theory, see: Wynn (1956), Section 2 - Initial conditions and algorithm setup
    if (n == static_cast<K>(0)) throw std::domain_error("n = 0 in the input");

    // Trivial case: order 0 returns the original partial sum
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // For theory, see: Wynn (1956), Section 3 - Algorithm construction and table size
    // Total number of entries needed in the epsilon table: k = 2*order + n
    K k = static_cast<K>(2) * order + n;

    // For theory, see: Wynn (1956), Section 3 - Epsilon table structure
    // The epsilon table is stored as a 4-row circular buffer to save memory
    std::vector<std::vector<T>> eps(4, std::vector<T>(k + static_cast<K>(1), utils::cast<T>(0.0, precision)));
    T a, a1, a2;
    a = a1 = a2 = utils::cast<T>(0.0, precision);

    // For theory, see: Wynn (1956), Eq. (2) - Initialization with partial sums
    // Initialize the bottom row with partial sums: ε₀⁽ᵐ⁾ = Sₙ for m = 0,1,...,k
    for (K i = static_cast<K>(0); i <= k; ++i) eps[3][i] = data.Sn.at(+i);

    K i1, i2;

    // For theory, see: Wynn (1956), Section 3 - Lozenge computation process
    // Build the epsilon table from bottom to top using the recurrence relation
    bool stable = false;
    while (k > static_cast<K>(0)) {
        for (K i = static_cast<K>(0); i != k; ++i) {
            i1 = i + static_cast<K>(1);
            i2 = i + static_cast<K>(2);

            // For theory, see: Wynn (1956), Eq. (4) - Main recurrence relation
            // εₖ₊₁⁽ᵐ⁾ = εₖ₋₁⁽ᵐ⁺¹⁾ + 1/(εₖ⁽ᵐ⁺¹⁾ - εₖ⁽ᵐ⁾)
            eps[0][i] = eps[2][i1] + utils::cast<T>(1.0, precision) / (eps[3][i1] - eps[3][i]);

            // For theory, see: Wynn (1964) - Numerical stability improvements
            // Additional checks and corrections for finite precision arithmetic
            stable = utils::isfinite(eps[0][i]);

            if (!stable && i2 <= k)  // Stability check and correction
            {
                a2 = utils::cast<T>(1.0, precision) / eps[2][i1];

                a1 = utils::cast<T>(1.0, precision) / (utils::cast<T>(1.0, precision) - (a2 * eps[2][i2]));
                a = eps[2][i2] * a1;

                a1 = utils::cast<T>(1.0, precision) / (utils::cast<T>(1.0, precision) - (a2 * eps[2][i]));
                a += eps[2][i] * a1;

                a1 = utils::cast<T>(1.0, precision) / (utils::cast<T>(1.0, precision) - (a2 * eps[0][i2]));
                a -= eps[0][i2] * a1;

                eps[0][i] = utils::cast<T>(1.0, precision) / eps[2][i1];
                eps[0][i] = utils::cast<T>(1.0, precision) / (utils::cast<T>(1.0, precision) + a * eps[0][i]);
                eps[0][i] = eps[0][i] * a;
            }

            // Final stability check after possible correction
            stable = utils::isfinite(eps[0][i]);

            // Fallback to previous value if correction fails
            if (!stable) eps[0][i] = eps[2][i];
        }

        // For theory, see: Wynn (1956), Section 3 - Table updating procedure
        // Circular shift of the rows in the buffer
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
    if (n % static_cast<K>(2) != static_cast<K>(0)) return eps[3][0];

    return eps[0][0];
}

}  // namespace algos
}  // namespace shanks

#endif
