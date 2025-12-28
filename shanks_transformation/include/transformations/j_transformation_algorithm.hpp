#ifndef J_TRANSFORMATION_ALGORITHM_HPP
#define J_TRANSFORMATION_ALGORITHM_HPP
#pragma once

#define J_TRASFORMATION_DEFAULT_MAX_ORDER 30
#define J_TRASFORMATION_DEFAULT_SAFEGUARD 1.0e-12
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

#include "series_acceleration.hpp"
#include <vector>

namespace shanks{ namespace algos{

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

    using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

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
        const float_type& safeguard = utils::cast<float_type>(J_TRASFORMATION_DEFAULT_SAFEGUARD)
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
     * @param data series_result<T> struct containing necessary information for algorithm
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

    K max_order_;            /**< Maximum order of J-transformation       */
    float_type safeguard_;   /**< Small value to prevent division by zero */

    inline T simple_formula(const K n, const series_result<T>& data) const;
    inline T recursive_formula(const K n, const K order, const series_result<T>& data) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::simple_formula(const K n, const series_result<T>& data) const {

    if (utils::abs(data.an[n + 1] + utils::cast<T>(safeguard_)) < safeguard_) return data.Sn[n];  // Fallback if denominator is too small

    return data.Sn[n + 1] - (data.Sn[n + 1] - data.Sn[n]) * (data.Sn[n + 1] - data.Sn[n]) / (data.an[n + 1] + utils::cast<T>(safeguard_));

}

template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::recursive_formula(const K n, const K order, const series_result<T>& data) const {
    // For higher orders, use recursive implementation
    // Initialize J0^{(k)} = S_{n+k}
    std::vector<T> J_prev(order + 1, utils::cast<T>(0.0));
    std::vector<T> J_curr(order + 1, utils::cast<T>(0.0));

    T delta_S, delta_term, term1, term2;
    delta_S = delta_term = term1 = term2 = utils::cast<T>(0.0);

    if constexpr (is_precisable<T>::value){
        const size_t precision = std::max(utils::get_precision(data.Sn[0]), utils::get_precision(data.an[0]));
        utils::set_precision(precision, delta_S, delta_term, term1, term2);
        utils::set_vec_precision(J_prev, precision);
        utils::set_vec_precision(J_curr, precision);
    }

    //init J_prev
    for (K i = 0; i <= order; ++i) J_prev[i] += data.Sn[n + i];

    K curr_J_size = J_prev.size() - 1;
    // Apply J-transformation recursively
    for (K k = 1; k <= order; ++k, --curr_J_size) {
        
        for (K i = 0; i < curr_J_size; ++i) {
        
            //delta_S = J_prev[i + 1] - J_prev[i];
            //Compute Δ_k^{(n)} term based on series terms
            //For J-transformation, we use a combination of series terms
            //First order uses next series term
        
            if (k == 1) delta_term = data.an[n + i + 1];
            else {
                // Higher orders use differences of previous J values
                term1 = (i + k     < data.an.size()) ? data.an[n + i + k    ] : utils::cast<T>(0.0);
                term2 = (i + k - 1 < data.an.size()) ? data.an[n + i + k - 1] : utils::cast<T>(0.0);
                delta_term = term1 - term2;
            }
        
            //delta_term += safeguard_ * ((delta_term >= 0) ? 1 : -1);
            //If denominator is too small, use linear interpolation
            
            if (utils::abs(delta_term) < safeguard_) J_curr[i] = J_prev[i + 1];
            else J_curr[i] = J_prev[i + 1] + delta_S * delta_S / delta_term;
        }

        J_prev.swap(J_curr);

        //If we've reduced to a single value, return it
        if (curr_J_size == 1) break;
    }

    // The final accelerated value is J_order^{(0)}
    return J_prev[0];
}


template <AcceptedLike T, UnsignedIntLike K>
T j_transformation_algorithm<T, K>::operator()(
    const K n,
    const K order,
    const series_result<T>& data
) const {
    // Check if we have enough data
    const K required_size = n + order + 1;

    if (data.Sn.size() < required_size) {
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" +
            utils::to_string(data.Sn.size()) +
            ", required at least " +
            utils::to_string(required_size)
        );
    }

    // Validate order parameter
    if (order > max_order_) {
        throw std::domain_error(
            "Requested order " + utils::to_string(order) +
            " exceeds maximum order " + utils::to_string(max_order_)
        );
    }

    // Base case: order 0 returns the partial sum directly
    if (order == 0) return data.Sn[n];

    const T result = (order == 1 ? simple_formula(n, data) : recursive_formula(n, order, data));

    if(!utils::isfinite(result)) throw std::overflow_error("division by zero");
    
    return result;
}

} //namespace shanks::algos
} //namespace shanksz

#endif