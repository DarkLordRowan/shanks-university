#ifndef BREZINSKI_THETA_ALGORITHM_HPP
#define BREZINSKI_THETA_ALGORITHM_HPP
#pragma once
/**
 * @file brezinski_theta_algorithm.hpp
 * @brief This file contains the declaration of the Theta Brezinski Algorithm class,
 *        which implements Brezinski's theta algorithm for sequence transformation.
 * @authors Yurov P.I., Bezzaborov A.A.
 */

 // For theory, see:
 // Brezinski, C. (1977). Acceleration de la Convergence en Analyse Numerique. Springer-Verlag.
 // Brezinski, C., & Redivo Zaglia, M. (2003). Extrapolation Methods: Theory and Practice.
 // Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of
 // Convergence and the Summation of Divergent Series. Computer Physics Reports, 1(1), 1-123.

#include "series_acceleration.hpp"
#include <vector>

namespace shanks{ namespace algos{

/**
 * @brief Brezinski's theta algorithm class template implementing a nonlinear sequence transformation.
 *
 * This algorithm implements Brezinski's theta transformation, which is an efficient
 * method for accelerating the convergence of slowly convergent sequences and series.
 * The algorithm uses a recursive approach with specific difference operations to
 * compute transformed sequence values. It is particularly known for its ability to
 * handle sequences where the errors behave like a linear combination of exponentials.
 * References:
 * - Brezinski, C. (1977). Acceleration de la Convergence en Analyse Numerique.
 * - Brezinski, C., & Redivo Zaglia, M. (2003). Extrapolation Methods: Theory and Practice.
 * - Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of
 *   Convergence and the Summation of Divergent Series.
 *
 * @authors Yurov P.I., Bezzaborov A.A.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all mathematical computations and storage of series terms
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 */
template <AcceptedLike T, UnsignedIntLike K>
class brezinski_theta_algorithm final : public series_acceleration<T, K>
{
public:

    /**
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     * @authors Yurov P.I., Bezzaborov A.A.
    */
    explicit brezinski_theta_algorithm() : series_acceleration<T, K>("brezinski theta algorithm") {}

    /**
     * @brief Fast implementation of Theta Brezinski algorithm.
     *
     * Computes the accelerated partial sum using Brezinski's theta transformation.
     * The algorithm uses recursive computation with specific difference operations
     * to accelerate convergence of the input sequence.
     *
     * For more information, see p. 277 10.2-4 in [https://arxiv.org/pdf/math/0306302.pdf]
     *
     * @param n The number of terms to use in the transformation
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (must be even)
     *        Valid values: order >= 0 and even (order % 2 == 0)
     *        Higher orders provide more acceleration but require more computation
     * @param data series_result<T> struct containing necessary information for algorithm
     * @return The accelerated partial sum after theta transformation
     * @throws std::domain_error if n=0 or order is odd
     * @throws std::overflow_error if division by zero occurs during computation
     */
    T operator()(
        const K n,
        const K order,
		const series_result<T>& data
    ) const override;
};

template <AcceptedLike T, UnsignedIntLike K>
T brezinski_theta_algorithm<T, K>::operator()(
    const K n,
    const K order,
	const series_result<T>& data
) const{

    // Calculate the number of terms required from the Sn vector
    const K required_size = static_cast<K>(3) * order / static_cast<K>(2) + static_cast<K>(1) + n;

    // Check if enough data is available in the partial sums vector
    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn is smaller then required for theta_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn must be at least " + utils::to_string(required_size));
    }

    // For theory, see: Brezinski (2003), Section 10.2, Theorem 10.2.1
    // Only even orders have mathematical meaning in the final result
    if (order & 1){ // is order odd?
        throw std::domain_error("order should be even number");
    }

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base case: if order is 0, return the original partial sum at index n.
    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	// For theory, see: Brezinski (2003), Section 10.2, Eq. (10.2.4)
	// Start computation with initial parameters
    // Determine the base size for auxiliary vectors
    const K base_size = static_cast<K>(3) * order / static_cast<K>(2) + static_cast<K>(1);

    // theta_odd and theta_even store intermediate results of the recursive transformation
    std::vector<T>  theta_odd(base_size, utils::cast<T>(0.0));
    std::vector<T> theta_even(base_size, utils::cast<T>(0.0));
    T delta = utils::cast<T>(0.0);

    // Initialize precision for types like mpreal
    if constexpr (is_precisable<T>::value){
        utils::set_vec_precision(theta_odd, utils::get_precision(data.Sn[0]));
        utils::set_vec_precision(theta_even, utils::get_precision(data.Sn[0]));
        utils::set_precision(utils::get_precision(data.Sn[0]), delta);
    }

    // Initialization: theta_0,j = S_j
    for(K j = static_cast<K>(0); j < base_size; ++j){
        theta_even[j] += data.Sn.at(n + j);
    }

    K j1, j2;

    // Iterative computation of theta levels
    for(K level = static_cast<K>(1); level <= order / static_cast<K>(2); ++level){

        // Transform and update the 'odd' vector (theta_2k+1)
        for(K j = static_cast<K>(0); j < base_size + static_cast<K>(2) - static_cast<K>(3) * level; ++j){

            j1 = j + static_cast<K>(1);
            // j2 = j + static_cast<K>(2); // unused here but preserved from original logic

            delta = theta_even[j1] - theta_even[j];

            // theta_2k+1,j = theta_2k+1,j+1 + 1 / (theta_2k,j+1 - theta_2k,j)
            theta_odd[j] = utils::fma(theta_odd[j1], delta, utils::cast<T>(1.0));
            theta_odd[j]/= delta;
        }

        // Transform and update the 'even' vector (theta_2k+2)
        for(K j = static_cast<K>(0); j < base_size - static_cast<K>(3) * level; ++j){

            j1 = j + static_cast<K>(1);
            j2 = j + static_cast<K>(2);

            delta = theta_odd[j2] - theta_odd[j1];

            theta_even[j] = theta_even[j1];
            theta_even[j]-= (theta_even[j2]-theta_even[j1]) * delta / (theta_odd[j1] - theta_odd[j] - delta);

        }
    }

    // Final check for validity of the result
    if(!utils::isfinite(theta_even[0])) throw std::overflow_error("division by zero");

    return theta_even[0];
}

} //namespace shanks::algos
} //namespace shanks

#endif
