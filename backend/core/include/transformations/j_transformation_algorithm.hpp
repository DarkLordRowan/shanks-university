#ifndef J_TRANSFORMATION_ALGORITHM_HPP
#define J_TRANSFORMATION_ALGORITHM_HPP
#pragma once

#define J_TRASFORMATION_DEFAULT_MAX_ORDER 30
#define J_TRASFORMATION_DEFAULT_SAFEGUARD 1.0e-12
/**
 * @file j_transformation_algorithm.hpp
 * @brief This file contains the declaration of the J-Transformation Algorithm class,
 *        which implements the J-transformation for accelerating series convergence.
 * @authors Mkhitaryan A.A., Gerasimov A.I.
 *
 * For theory, see:
 * Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of
 * Convergence and the Summation of Divergent Series. Computer Physics Reports, 1(1), 1-123.
 * Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 */

namespace shanks {
namespace algos {

/**
 * @brief J-Transformation algorithm class template implementing nonlinear sequence transformation.
 *
 * The J-transformation is a powerful nonlinear sequence transformation that can
 * accelerate the convergence of slowly convergent and divergent series. It is
 * particularly effective for alternating series and series with factorial growth.
 * It uses a recursive scheme to progressively refine the estimate of the sum.
 *
 * References:
 * - Weniger, E. J. (2003). Nonlinear Sequence Transformations.
 * - Sidi, A. (2003). Practical Extrapolation Methods: Theory and Applications.
 *
 * @authors Mkhitaryan A.A., Gerasimov A.I.
 *
 * @tparam T Floating-point type for series elements (must satisfy AcceptedLike)
 *           Represents numerical precision (float, double, long double, or arbitrary precision)
 *           Used for all mathematical computations and storage of series terms.
 * @tparam K Unsigned integral type for indices and order (must satisfy UnsignedIntLike)
 *           Used for counting terms, indexing operations, and transformation order.
 */
template <AcceptedLike T, UnsignedIntLike K>
class j_transformation_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

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
        const K max_order = static_cast<K>(J_TRASFORMATION_DEFAULT_MAX_ORDER),
        const float_type& safeguard = utils::cast<float_type, double>()(J_TRASFORMATION_DEFAULT_SAFEGUARD))
        : series_acceleration<T, K>("j transformation algorithm"),
          max_order_(max_order > 0 ? max_order : 1),
          safeguard_(safeguard) {}

    /**
     * @brief Computes the accelerated partial sum using J-transformation.
     *
     * The J-transformation applies a nonlinear recursive scheme to the sequence
     * of partial sums. It is defined by the recursive relation:
     * J_k^{(n)} = J_{k-1}^{(n+1)} + (J_{k-1}^{(n+1)} - J_{k-1}^{(n)}) / Δ_k^{(n)}
     * where Δ_k^{(n)} is a carefully chosen difference operator.
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n The starting index for the transformation
     *        Valid values: n >= max_order_
     *        Higher starting indices provide more stability
     * @param order The order of J-transformation to apply
     *        Valid values: 0 <= order <= max_order_
     *        Higher orders provide more acceleration
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum
     * @throws std::out_of_range if data size is insufficient
     * @throws std::domain_error if numerical issues occur
     * @throws std::overflow_error if a division by zero or non-finite result occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    inline std::size_t how_much(const std::size_t n, const std::size_t order) const {
        return n + order + std::size_t{1};
    }

private:
    K max_order_;          /**< Maximum order of J-transformation       */
    float_type safeguard_; /**< Small value to prevent division by zero */

    /**
     * @brief Simple (first-order) J-transformation formula implementation.
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n The base index.
     * @param data The series data.
     * @return T The result of the first-order transformation.
     */
    inline T simple_formula(const K n, const series_result<T>& data) const;

    /**
     * @brief Recursive implementation for higher-order J-transformations.
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n The base index.
     * @param order The transformation order.
     * @param data The series data.
     * @return T The result of the higher-order transformation.
     */
    inline T recursive_formula(const K n, const K order, const series_result<T>& data) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::simple_formula(const K n, const series_result<T>& data) const {
    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    // Numerical stability check for the denominator
    if (utils::math<T>::abs(data.an[n + 1] + utils::cast<T, float_type>()(safeguard_, precision)) < safeguard_)
        return data.Sn[n];

    // Standard first-order J-transformation formula
    return data.Sn[n + 1] - (data.Sn[n + 1] - data.Sn[n]) * (data.Sn[n + 1] - data.Sn[n]) /
                                (data.an[n + 1] + utils::cast<T, float_type>()(safeguard_, precision));
}

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::recursive_formula(const K n, const K order, const series_result<T>& data) const {
    const size_t precision =
        std::max(utils::helpers<T>::get_precision(data.Sn[0]), utils::helpers<T>::get_precision(data.an[0]));

    // For higher orders, use recursive implementation
    // Initialize J0^{(k)} = S_{n+k}
    // Auxiliary vectors for storing transformation results at consecutive levels
    std::vector<T> J_prev(order + 1, utils::cast<T, int>()(0, precision));
    std::vector<T> J_curr(order + 1, utils::cast<T, int>()(0, precision));

    T delta_S, delta_term, term1, term2;
    delta_S = delta_term = term1 = term2 = utils::cast<T, int>()(0, precision);

    // Base initialization: J_0,i = S_n+i
    for (K i = 0; i <= order; ++i) J_prev[i] += data.Sn[n + i];

    K curr_J_size = J_prev.size() - 1;

    // Outer loop for the transformation order k
    for (K k = 1; k <= order && curr_J_size > 1; ++k, --curr_J_size) {
        // Inner loop over the sequence index i
        for (K i = 0; i < curr_J_size; ++i) {
            // delta_S = J_prev[i + 1] - J_prev[i];
            // Compute Δ_k^{(n)} term based on series terms
            // For J-transformation, we use a combination of series terms
            // First order uses next series term

            // Determine the difference operator term based on the current order k
            if (k == 1)
                delta_term = data.an[n + i + 1];
            else {
                // For higher orders, use differences of the series terms
                term1 = (i + k < data.an.size()) ? data.an[n + i + k] : utils::cast<T, int>()(0, precision);
                term2 = (i + k - 1 < data.an.size()) ? data.an[n + i + k - 1] : utils::cast<T, int>()(0, precision);
                delta_term = term1 - term2;
            }

            // delta_term += safeguard_ * ((delta_term >= 0) ? 1 : -1);
            // If denominator is too small, use linear interpolation

            // Check for potential division by zero using the safeguard
            if (utils::math<T>::abs(delta_term) < safeguard_)
                J_curr[i] = J_prev[i + 1];
            else
                J_curr[i] = J_prev[i + 1] + delta_S * delta_S / delta_term;
        }

        // Move to the next transformation level
        J_prev.swap(J_curr);
    }

    // The accelerated value is found at the first position of the highest level vector
    return J_prev[0];
}

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {
    // Check if we have enough data
    const std::size_t required_size = j_transformation_algorithm<T, K>::how_much(n, order);

    if (data.Sn.size() < required_size) {
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" + utils::helpers<std::size_t>::to_string(data.Sn.size()) +
            ", required at least " + utils::helpers<std::size_t>::to_string(required_size));
    }

    // Validate order parameter
    if (order > max_order_) {
        throw std::domain_error("Requested order " + utils::helpers<K>::to_string(order) + " exceeds maximum order " +
                                utils::helpers<K>::to_string(max_order_));
    }

    // Base case: order 0 returns the partial sum directly
    if (order == 0) return data.Sn[n];

    // Delegate to either the simplified or the recursive implementation
    const T result = (order == 1 ? simple_formula(n, data) : recursive_formula(n, order, data));

    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
