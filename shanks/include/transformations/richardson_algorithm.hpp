#ifndef RICHARDSON_ALGORITHM_HPP
#define RICHARDSON_ALGORITHM_HPP
#pragma once
/**
 * @file richardson_algorithm.hpp
 * @brief This file contains the definition of the Richardson transformation class
 *        for series acceleration using Richardson extrapolation technique.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

 // For theory, see:
 // Richardson, L.F. (1911). The approximate arithmetical solution by finite differences
 // of physical problems including differential equations with an application to the stresses
 // in a masonry dam. Philosophical Transactions of the Royal Society of London. Series A, 210, 459-470.
 // Richardson, L.F., & Gaunt, J.A. (1927). The deferred approach to the limit.
 // Philosophical Transactions of the Royal Society of London. Series A, 226, 299-361.


#include "series_acceleration.hpp"
#include <vector>

namespace shanks{ namespace algos{

/**
 * @brief Richardson transformation class for series acceleration
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * This class implements the Richardson extrapolation method for accelerating
 * the convergence of series. The method uses polynomial extrapolation to
 * eliminate lower-order error terms in the sequence of partial sums.
 *
 * References:
 * - Richardson, L.F. (1911). The approximate arithmetical solution by finite differences
 * - Richardson, L.F., & Gaunt, J.A. (1927). The deferred approach to the limit
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 *
 * @tparam K Unsigned integral type for indices and counting operations
 *           - Purpose: Used for term counting, indexing, and loop control
 *           - Valid values: Any std::unsigned_integral type (unsigned int, unsigned long, size_t)
 *           - Constraints: Must satisfy std::unsigned_integral concept
 *           - Example usage: Number of terms, transformation order, array indices
 */
template <AcceptedLike T, UnsignedIntLike K>
class richardson_algorithm final : public series_acceleration<T, K>
{
public:

    /**
     * @brief Parameterized constructor to initialize the Richardson transformation for series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
    */
    explicit richardson_algorithm() : series_acceleration<T, K>("richardson") {}

    /**
     * @brief Richardson transformation for series acceleration.
     *
     * Computes the accelerated partial sum using Richardson extrapolation technique.
     * The method constructs an extrapolation table to eliminate lower-order error terms
     * and improve convergence rate.
     *
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param n The number of terms to use in the transformation
     *        - Purpose: Determines how many partial sums are considered for acceleration
     *        - Valid values: n > 0 (requires at least 1 term)
     *        - Effect: Higher values use more terms but provide better acceleration
     *        - Typical range: 5-20 terms depending on series convergence
     *
     * @param order The order of transformation
     *        - Purpose: Specifies the transformation order (unused in this implementation)
     *        - Valid values: order >= 0 (maintained for interface consistency)
     *        - Note: This parameter is unused but required by the base class interface
     *
     * @param data series_result<T> struct containing necessary information for algorithm
     *
     * @return The accelerated partial sum after Richardson transformation
     *         - Type: Same as template parameter T
     *         - Meaning: Improved approximation of the series sum using n terms
     *
     * @throws std::domain_error if n=0 is provided as input
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator()(
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;
};

template <AcceptedLike T, UnsignedIntLike K>
T richardson_algorithm<T, K>::operator()(
    const K n,
    const K order,
    const series_result<T>& data
) const {

    // Ensure we have enough partial sums to perform the extrapolation
    const K required_size = n + static_cast<K>(1);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn smaller then required for richardson_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn must be at least " + utils::to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    // in the method we don't use order, it's only a stub
    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");

    // For theory, see: Richardson (1911) - construction of extrapolation table
    // Storage for Richardson extrapolation table with two rows for efficient computation
    std::vector<std::vector<T>> e(
        2,
        std::vector<T>(n + static_cast<K>(1), utils::cast<T>(0))
    ); // Two vectors n + 1 length containing Richardson table next and previous
    T a = utils::cast<T>(1.0);
    T b = utils::cast<T>(0.0);

    // Initialize precision if the type T supports it
    if constexpr (is_precisable<T>::value){
        utils::set_vec_precision(e[0], utils::get_precision(data.Sn[0]));
        utils::set_vec_precision(e[1], utils::get_precision(data.Sn[0]));
        utils::set_precision(utils::get_precision(data.Sn[0]), a,b);
    }

    // Initialization: Load the first row with partial sums
	// For theory, see: Richardson (1911), Eq. (2) - initialization with partial sums
    for (K i = static_cast<K>(0); i <= n; ++i)
        e[0][i] = data.Sn.at(i);

    // Main recursion loop for the Richardson extrapolation scheme
	// For theory, see: Richardson & Gaunt (1927), Section 3 - recursive extrapolation
	// Richardson extrapolation recursion: Tₖ⁽ⁿ⁾ = (4ᵏTₖ₋₁⁽ⁿ⁺¹⁾ - Tₖ₋₁⁽ⁿ⁾) / (4ᵏ - 1)
    for (K l = static_cast<K>(1); l <= n; ++l) {
        a *= utils::cast<T>(4);     // 4ᵏ factor
        b = a - utils::cast<T>(1);  // (4ᵏ - 1) denominator

        for (K m = l; m <= n; ++m){
            // For theory, see: Richardson & Gaunt (1927), Eq. (3.5)
            // Richardson extrapolation formula: Tₖ⁽ⁿ⁾ = (4ᵏTₖ₋₁⁽ⁿ⁺¹⁾ - Tₖ₋₁⁽ⁿ⁾) / (4ᵏ - 1)
            e[1][m] = utils::fma(a, e[0][m], -e[0][m - static_cast<K>(1)]);
            e[1][m]/= b;
        }

        std::swap(e[0], e[1]); // Swap rows for next iteration
    }

    // Final check for validity. The result selection logic depends on the number of swaps.
    if(!utils::isfinite(e[n & static_cast<K>(1)][n])){
        throw std::overflow_error("division by zero");
    }

    return e[n & static_cast<K>(1)][n];

}

} //namespace shanks::algos
} //namespace shanks

#endif
