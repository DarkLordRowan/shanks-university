/**
 * @file j_transformation_algorithm.hpp
 * @brief This file contains the declaration of the J-Transformation Algorithm class,
 *        which implements the J-transformation for accelerating series convergence.
 *
 * For theory, see:
 * Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of
 * Convergence and the Summation of Divergent Series. Computer Physics Reports, 1(1), 1-123.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 */

#pragma once

#include "series_acceleration.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>
#include <limits>

/**
 * @brief J-Transformation algorithm class template implementing nonlinear sequence transformation.
 *
 * @authors Mkhitaryan A.A., Gerasimov A.I.
 *
 * The J-transformation is a powerful nonlinear sequence transformation that can
 * accelerate the convergence of slowly convergent and divergent series. It is
 * particularly effective for alternating series and series with factorial growth.
 *
 * References:
 * - Weniger, E. J. (2003). Nonlinear Sequence Transformations.
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike)
 *           Represents numerical precision (float, double, long double, float_precision)
 *           Used for all mathematical computations and storage of series terms
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike)
 *           Used for counting terms, indexing operations, and transformation order
 */
template <AcceptedLike T, UnsignedIntLike K>
class j_transformation_algorithm final : public series_acceleration<T, K>
{
public:
    /**
     * @brief Parameterized constructor to initialize the J-Transformation Algorithm.
     *
     * Initializes the algorithm with specified maximum order and safeguard parameter.
     * The J-transformation uses a recursive scheme that builds upon previous transforms.
     *
     * @param max_order Maximum order of J-transformation (default: 5)
     *        Valid values: max_order >= 1
     *        Higher orders may provide better acceleration but require more computation
     * @param safeguard Small value to prevent division by zero (default: 1e-12)
     */
    explicit j_transformation_algorithm(
        K max_order = 5,
        T safeguard = static_cast<T>(1e-12)
    ) : series_acceleration<T, K>("j transformation algorithm"),
        max_order_(max_order > 0 ? max_order : 1),
        safeguard_(safeguard)
    {}

    /**
     * @brief Computes the accelerated partial sum using J-transformation.
     *
     * The J-transformation applies a nonlinear recursive scheme to the sequence
     * of partial sums. It is defined by the recursive relation:
     * J_k^{(n)} = J_{k-1}^{(n+1)} + (J_{k-1}^{(n+1)} - J_{k-1}^{(n)}) / Δ_k^{(n)}
     * where Δ_k^{(n)} is a carefully chosen difference operator.
     *
     * @param n The starting index for the transformation
     *        Valid values: n >= max_order_
     *        Higher starting indices provide more stability
     * @param order The order of J-transformation to apply
     *        Valid values: 0 <= order <= max_order_
     *        Higher orders provide more acceleration
     * @param data Series result containing Sn and an vectors
     * @return The accelerated partial sum
     * @throws std::out_of_range if data size is insufficient
     * @throws std::domain_error if numerical issues occur
     */
    T operator()(
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;

private:
    K max_order_;   ///< Maximum order of J-transformation
    T safeguard_;   ///< Small value to prevent division by zero
};

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::operator()(
    const K n,
    const K order,
    const series_result<T>& data
) const {
    // Check if we have enough data
    K required_size = n + order + 1;
    if (data.Sn.size() < required_size) {
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" +
            std::to_string(data.Sn.size()) +
            ", required at least " +
            std::to_string(required_size)
        );
    }

    // Validate order parameter
    if (order > max_order_) {
        throw std::domain_error(
            "Requested order " + std::to_string(order) +
            " exceeds maximum order " + std::to_string(max_order_)
        );
    }

    // Base case: order 0 returns the partial sum directly
    if (order == 0) {
        return data.Sn[n];
    }

    // For order 1, J-transformation reduces to a simple formula
    if (order == 1) {
        T S_n = data.Sn[n];
        T S_n1 = data.Sn[n + 1];
        T a_n1 = data.an[n + 1];  // Next term in series

        T denominator = a_n1 + safeguard_;

        if (std::abs(denominator) < safeguard_) {
            return data.Sn[n];  // Fallback if denominator is too small
        }

        return S_n1 - (S_n1 - S_n) * (S_n1 - S_n) / denominator;
    }

    // For higher orders, use recursive implementation
    // Initialize J0^{(k)} = S_{n+k}
    std::vector<T> J_prev;
    for (K i = 0; i <= order; ++i) {
        J_prev.push_back(data.Sn[n + i]);
    }

    // Apply J-transformation recursively
    for (K k = 1; k <= order; ++k) {
        std::vector<T> J_curr(J_prev.size() - 1, static_cast<T>(0));

        for (K i = 0; i < J_curr.size(); ++i) {
            T delta_S = J_prev[i + 1] - J_prev[i];

            // Compute Δ_k^{(n)} term based on series terms
            // For J-transformation, we use a combination of series terms
            T delta_term;
            if (k == 1) {
                // First order uses next series term
                delta_term = data.an[n + i + 1];
            } else {
                // Higher orders use differences of previous J values
                T term1 = (i + k < data.an.size()) ? data.an[n + i + k] : static_cast<T>(0);
                T term2 = (i + k - 1 < data.an.size()) ? data.an[n + i + k - 1] : static_cast<T>(0);
                delta_term = term1 - term2;
            }

            delta_term += safeguard_ * ((delta_term >= 0) ? 1 : -1);

            if (std::abs(delta_term) < safeguard_) {
                // If denominator is too small, use linear interpolation
                J_curr[i] = J_prev[i + 1];
            } else {
                J_curr[i] = J_prev[i + 1] + delta_S * delta_S / delta_term;
            }
        }

        J_prev = std::move(J_curr);

        // If we've reduced to a single value, return it
        if (J_prev.size() == 1) {
            break;
        }
    }

    // The final accelerated value is J_order^{(0)}
    return J_prev[0];
}