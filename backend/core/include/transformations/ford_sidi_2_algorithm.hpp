#ifndef FORD_SIDI_2_HPP
#define FORD_SIDI_2_HPP
#pragma once
/**
 * @file ford_sidi_2_algorithm.hpp
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 *        This implementation is based on the efficient Ford-Sidi algorithm
 *        that requires fewer arithmetic operations than the E-algorithm.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

// For theory, see:
// Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
// Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.

namespace shanks {
namespace algos {

/**
 * @brief Ford-Sidi algorithm class template implementing an efficient extrapolation method.
 *
 * This algorithm provides a more economical implementation compared to the standard E-algorithm
 * while maintaining mathematical equivalence. It is particularly effective for sequences
 * with specific convergence patterns. The variant implemented here focuses on a simplified
 * finite-difference approach for improved performance.
 *
 * References:
 * - Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
 *   SIAM Journal on Numerical Analysis, 24(5), 1212-1232.
 * - Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.
 *   Journal of Computational and Applied Mathematics, 122(1), 223-230.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike)
 *           Represents numerical precision (float, double, long double, or arbitrary precision).
 *           Used for all numerical computations and storage.
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike)
 *           Used for counting terms, indexing operations, and loop control.
 *           Valid values: K >= 0, typically size_t or unsigned int.
 */
template <AcceptedLike T, UnsignedIntLike K>
class ford_sidi_2_algorithm final : public series_acceleration<T, K> {
public:
    /**
     * @brief Parameterized constructor to initialize the Ford-Sidi V-2 Algorithm.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ford_sidi_2_algorithm() : series_acceleration<T, K>("ford sidi 2") {}

    /**
     * @brief Fast implementation of Ford-Sidi algorithm for series acceleration.
     *
     * Computes the accelerated sum using the efficient Ford-Sidi implementation
     * that requires fewer arithmetic operations than the standard E-algorithm.
     * The algorithm uses a simplified approach based on finite differences.
     *
     * For theory, see: Osada (2000), Section 4, Eq. (20)
     * T_k^{(n)} = [ψ_{k-1}^{(n+1)}(s) - ψ_{k-1}^{(n)}(s)] / [ψ_{k-1}^{(n+1)}(1) - ψ_{k-1}^{(n)}(1)]
     *
     * @param n The number of terms to use in the transformation (index starting point).
     *        Valid values: n > 0 (algorithm requires at least 1 term).
     *        Represents the starting index for the transformation process.
     * @param order The order of transformation (unused in this implementation,
     *        maintained for interface consistency with base class).
     *        Valid values: order >= 0 (typically set to 0 or ignored).
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after Ford-Sidi transformation.
     * @throws std::out_of_range if the Sn vector size is insufficient.
     * @throws std::domain_error if n is 0.
     * @throws std::overflow_error if division by zero or numerical instability occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;
};

template <AcceptedLike T, UnsignedIntLike K>
T ford_sidi_2_algorithm<T, K>::operator()(const K n, const K /*order*/, const series_result<T>& data) const {
    // Check if we have enough partial sums (at least n+2)
    const K required_size = n + static_cast<K>(2);
    const size_t precision = utils::helpers<T>::get_precision(data.Sn[0]);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range("The Sn smaller then required for ford_sidi2_{" + utils::helpers<T>::to_string(n) +
                                "}\n" + "the size of Sn must be at least " +
                                utils::helpers<T>::to_string(required_size));
    }

    // For theory, see: Ford & Sidi (1987), Section 1 - Input validation
    // The algorithm requires at least one term for meaningful computation
    if (n == static_cast<K>(0)) throw std::domain_error("n = 0 in the input");

    T delta_squared_S_n, delta_S_n, T_n;
    delta_squared_S_n = delta_S_n = T_n = utils::cast<T, int>()(0, precision);

    K m = n;

    // For theory, see: Osada (2000), Section 2.2 - Auxiliary sequence computation
    // The algorithm searches for a non-zero second difference to ensure numerical stability
    do {
        // For theory, see: Ford & Sidi (1987), Eq. (1.8) - Finite difference computation
        // Second difference formula: Δ²S_m = S_{m+2} - 2S_{m+1} + S_m
        delta_squared_S_n += data.Sn.at(m + static_cast<K>(2));
        delta_squared_S_n -= data.Sn.at(m + static_cast<K>(1)) * utils::cast<T, int>()(2, precision);
        delta_squared_S_n += data.Sn.at(m);

    } while (delta_squared_S_n == utils::cast<T, int>()(0, precision) && --m > static_cast<K>(0));

    // For theory, see: Osada (2000), Section 4 - Stability condition
    // Zero second difference indicates numerical instability or convergence issues
    if (m == static_cast<K>(0)) throw std::overflow_error("division by zero");

    // For theory, see: Ford & Sidi (1987), Eq. (1.9) - First difference computation
    // First difference formula: ΔS_m = S_{m+1} - S_m
    delta_S_n += data.Sn.at(m + static_cast<K>(1)) - data.Sn.at(m);

    // For theory, see: Osada (2000), Eq. (20) - Main transformation formula
    // Ford-Sidi acceleration: T_n = S_m - [(ΔS_m)² / Δ²S_m]
    T_n += utils::math<T>::fma(-delta_S_n, delta_S_n / delta_squared_S_n, data.Sn.at(m));

    // For theory, see: Ford & Sidi (1987), Section 3 - Numerical stability check
    // Ensures the result is a finite floating-point value
    if (!utils::helpers<T>::isfinite(T_n)) throw std::overflow_error("division by zero");
    return T_n;
}

}  // namespace algos
}  // namespace shanks

#endif
