/**
 * @file richardson_algorithm.hpp
 * @brief This file contains the definition of the Richardson transformation class
 *        for series acceleration using Richardson extrapolation technique.
 */

 // For theory, see:
 // Richardson, L.F. (1911). The approximate arithmetical solution by finite differences 
 // of physical problems including differential equations with an application to the stresses 
 // in a masonry dam. Philosophical Transactions of the Royal Society of London. Series A, 210, 459-470.
 // Richardson, L.F., & Gaunt, J.A. (1927). The deferred approach to the limit. 
 // Philosophical Transactions of the Royal Society of London. Series A, 226, 299-361.

#pragma once

#include "../series_acceleration.hpp"
#include <vector>   // Include the vector library
#include <cmath>    //Include for fma, isfinite

/**
 * @brief Richardson transformation class for series acceleration
 *
 * @authors Trudolyubov N.A., Pavlova A.R.
 *
 * This class implements the Richardson extrapolation method for accelerating
 * the convergence of series. The method uses polynomial extrapolation to
 * eliminate lower-order error terms in the sequence of partial sums.
 *
 * References:
 * - Richardson, L.F. (1911). The approximate arithmetical solution by finite differences
 * - Richardson, L.F., & Gaunt, J.A. (1927). The deferred approach to the limit
 *
 * @tparam T Floating-point type for series elements and computations
 *           - Purpose: Represents numerical precision for all calculations
 *           - Valid values: Any std::floating_point type (float, double, long double)
 *           - Constraints: Must satisfy std::floating_point concept
 *           - Example usage: Stores partial sums, transformation results, and intermediate values
 *
 * @tparam K Unsigned integral type for indices and counting operations
 *           - Purpose: Used for term counting, indexing, and loop control
 *           - Valid values: Any std::unsigned_integral type (unsigned int, unsigned long, size_t)
 *           - Constraints: Must satisfy std::unsigned_integral concept
 *           - Example usage: Number of terms, transformation order, array indices
 *
 * @tparam series_templ Type of series object to accelerate
 *           - Purpose: Encapsulates the mathematical series to be transformed
 *           - Valid values: Any type providing the required interface
 *           - Required interface:
 *               - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
 *           - Example usage: Convergent series with known partial sums
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class richardson_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

    /**
     * @brief Parameterized constructor to initialize the Richardson transformation for series.
     * @param series The series class object to be accelerated
     *        - Purpose: Provides access to series partial sums
     *        - Valid values: Any object implementing the required series interface
     *        - Constraints: Must be copy-constructible and provide S_n method
     *        - Example: Mathematical series with known convergence properties
     */
    explicit richardson_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief Richardson transformation for series acceleration
     *
     * Computes the accelerated partial sum using Richardson extrapolation technique.
     * The method constructs an extrapolation table to eliminate lower-order error terms
     * and improve convergence rate.
     *
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
     * @return The accelerated partial sum after Richardson transformation
     *         - Type: Same as template parameter T
     *         - Meaning: Improved approximation of the series sum using n terms
     *
     * @throws std::domain_error if n=0 is provided as input
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator() (K n, K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T richardson_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    // in the method we don't use order, it's only a stub 
    if (n == static_cast<K>(0)) 
        throw std::domain_error("n = 0 in the input");

    // For theory, see: Richardson (1911) - construction of extrapolation table
    // Storage for Richardson extrapolation table with two rows for efficient computation
    std::vector<std::vector<T>> e(
        2,
        std::vector<T>(
            n + static_cast<K>(1),
            static_cast<T>(0)
        )
    ); // Two vectors n + 1 length containing Richardson table next and previous 

    // For theory, see: Richardson (1911), Eq. (2) - initialization with partial sums
    // Initialize the first row of the extrapolation table with partial sums
    for (K i = static_cast<K>(0); i <= n; ++i)
        e[0][i] = this->series->S_n(i);

    // The Richardson method main function 
    T a, b;
    a = static_cast<T>(1);

    // For theory, see: Richardson & Gaunt (1927), Section 3 - recursive extrapolation
    // Richardson extrapolation recursion: Tₖ⁽ⁿ⁾ = (4ᵏTₖ₋₁⁽ⁿ⁺¹⁾ - Tₖ₋₁⁽ⁿ⁾) / (4ᵏ - 1)
    for (K l = static_cast<K>(1); l <= n; ++l) {
        a *= static_cast<T>(4);     // 4ᵏ factor
        b = a - static_cast<T>(1);  // (4ᵏ - 1) denominator

        for (K m = l; m <= n; ++m){
            // For theory, see: Richardson & Gaunt (1927), Eq. (3.5)
            // Richardson extrapolation formula: Tₖ⁽ⁿ⁾ = (4ᵏTₖ₋₁⁽ⁿ⁺¹⁾ - Tₖ₋₁⁽ⁿ⁾) / (4ᵏ - 1)
            e[1][m] = fma(a, e[0][m], -e[0][m - static_cast<K>(1)]); 
            e[1][m]/= b;
        }

        std::swap(e[0], e[1]); // Swap rows for next iteration
    }

    if (!isfinite(e[n & static_cast<K>(1)][n])) // get n & 1, cause if n is even, result is e[0][n], if n is odd, result is e[1][n]
        throw std::overflow_error("division by zero");

    return e[n & static_cast<K>(1)][n];
}