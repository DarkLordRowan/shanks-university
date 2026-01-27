#ifndef LUBKIN_W_ALGORITHM_HPP
#define LUBKIN_W_ALGORITHM_HPP
#pragma once
/**
 * @file lubkin_w_algorithm.hpp
 * @brief This file contains the definition of Lubkin's W-transformation,
 *        a sequence acceleration method effective for both linear and logarithmic convergence.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

// For theory, see:
// Lubkin, S. (1952). A method of summing infinite series. J. Res. Nat. Bur. Standards, 48:228-254.
// Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation. Mathematical Tables and Other Aids to
// Computation. Osada, N. (1992). A method for obtaining sequence transformations. IMA Journal of Numerical Analysis,
// 12:85-94. Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. Cambridge University Press.
// (Chapter 15.4)

namespace shanks {
namespace algos {

/**
 * @brief Lubkin's W-transformation class template implementing a powerful sequence acceleration method.
 *
 * This algorithm accelerates the convergence of sequences by applying Lubkin's W-transformation,
 * which is particularly effective for both linearly and logarithmically convergent sequences.
 * It is based on a specific rational function representation of the sequence and can
 * significantly improve the rate of convergence for many types of infinite series.
 *
 * References:
 * - Lubkin, S. (1952). A method of summing infinite series. J. Res. Nat. Bur. Standards.
 * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * - Osada, N. (1992). A method for obtaining sequence transformations.
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications. (Chapter 15.4)
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike).
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class lubkin_w_algorithm final : public series_acceleration<T, K> {
public:
    /**
     * @brief Parameterized constructor to initialize the Lubkin W-transformation.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit lubkin_w_algorithm() : series_acceleration<T, K>("lubkin W transformation") {}

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
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n The number of terms to use in the transformation
     *        Valid values: n ≥ 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (number of iterations)
     *        Valid values: order ≥ 0
     *        Higher orders provide more acceleration but require more terms
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after Lubkin transformation
     * @throws std::domain_error if negative order is provided
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;
};

template <AcceptedLike T, UnsignedIntLike K>
T lubkin_w_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Ensure there is sufficient data in the Sn vector for the requested order and starting index
    const K required_size = n + static_cast<K>(3) * order + static_cast<K>(1);
    const size_t precision = utils::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range("The Sn smaller then required for W_{" + utils::to_string(order) + "}^{" +
                                utils::to_string(n) + "}\n" + "the size of Sn must be at least " +
                                utils::to_string(required_size));
    }

    // Trivial case: order 0 returns the original partial sum at index n
    if (order == static_cast<K>(0)) return data.Sn.at(n);

    // For theory, see: Wynn (1956), Section 3 - Table construction
    // Storage scheme requires 3n+1 terms for order n transformation
    const K base_size = static_cast<K>(3) * order + static_cast<K>(1);

    // Working vector to store intermediate transformation values
    std::vector<T> W(base_size, utils::cast<T>(0, precision));

    T Wo0, Wo1, Wo2, Woo1, Woo2;
    Wo0 = Wo1 = Wo2 = Woo1 = Woo2 = utils::cast<T>(0, precision);

    // Load initial partial sums into the working vector starting from index n
    for (K i = static_cast<K>(0); i < base_size; ++i) W[i] += data.Sn.at(+i);

    K j1, j2, j3;  // Index variables

    // For theory, see:
    // - Lubkin (1952), Eq. (5.2)
    // - Osada (1992), Theorem 2 and Eq. (5.2)
    // - Sidi (2003), Chapter 15.4, Eq. (15.4.1)
    // Iterative application of the W-transformation
    for (K level = static_cast<K>(1); level <= order; ++level) {
        for (K j = static_cast<K>(0); j < base_size - level * static_cast<K>(3); ++j) {
            j1 = j + static_cast<K>(1);
            j2 = j + static_cast<K>(2);
            j3 = j + static_cast<K>(3);

            // For theory, see: Lubkin (1952), Eq. (5.2)
            // First differences: ΔS_j = S_{j+1} - S_j
            Wo0 = W[j1] - W[j];   // ΔS_j
            Wo1 = W[j2] - W[j1];  // ΔS_{j+1}
            Wo2 = W[j3] - W[j2];  // ΔS_{j+2}

            // For theory, see: Osada (1992), Eq. (5.2)
            // Numerator: (ΔS_n·ΔS_{n-1}·Δ²S_{n-2}) = ΔS_{j+1}·(ΔS_{j+1} - ΔS_j)·ΔS_j
            // Denominator: (ΔS_n·Δ²S_{n-2} - ΔS_{n-2}·Δ²S_{n-1}) = ΔS_{j+2}·(ΔS_{j+1} - ΔS_j) - ΔS_j·(ΔS_{j+2} -
            // ΔS_{j+1})
            Woo1 = Wo0 * (Wo2 - Wo1);  // ΔS_j·(ΔS_{j+2} - ΔS_{j+1})
            Woo2 = Wo2 * (Wo1 - Wo0);  // ΔS_{j+2}·(ΔS_{j+1} - ΔS_j)

            // For theory, see: Lubkin (1952), Main transformation formula
            // W_n = S_{n+1} - [Numerator] / [Denominator]
            // Optimized computation using fused multiply-add for better numerical stability
            W[j] = utils::fma(-Wo1, Woo1 / (Woo2 - Woo1), W[j1]);
        }
    }

    // Numerical stability check
    if (!utils::isfinite(W[0])) throw std::overflow_error("division by zero");

    return W[0];
}

}  // namespace algos
}  // namespace shanks

#endif
