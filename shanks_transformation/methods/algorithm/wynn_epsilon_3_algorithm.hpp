/**
 * @file wynn_epsilon_3_algorithm.hpp
 * @brief This file contains the declaration of the third implementation of Wynn's Epsilon Algorithm.
 *
 * For theory, see:
 * Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 *   Mathematical Tables and Other Aids to Computation, 10(54), 91-96.
 * Wynn, P. (1962). A comparison technique for the numerical transformation of slowly convergent series based on the use of rational functions.
 *   Numerische Mathematik, 4(1), 8-14.
 * Wynn, P. (1964). General Purpose Vector Epsilon Algorithm ALGOL Procedures.
 *   Numerische Mathematik, 6(1), 22-36.
 */

#pragma once

#include "../series_acceleration.hpp"
#include <cmath>        //Include for isfinite
#include <vector>       // Include the vector library


 /**
  * @brief Wynn's Epsilon Algorithm (Third Implementation) class template.
  *
  * This implementation combines numerical stability enhancements with efficient memory usage.
  * It computes accelerated partial sums using the epsilon algorithm with threshold-based
  * error control and adaptive table management.
  *
  * Template Parameters:
  * @tparam T Floating-point type for series elements and computations.
  *           Must satisfy FloatLike. Represents numerical precision (float, double, long double).
  *           Determines the precision of all arithmetic operations and storage.
  * @tparam K Unsigned integral type for indices, counts, and sizes.
  *           Must satisfy std::unsigned_integral. Used for indexing terms, table sizes, and loop counters.
  *           Typical types: unsigned int, std::size_t.
  * @tparam series_templ Type of the series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term aₙ
  *           - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
  *           The series object encapsulates the sequence whose convergence is to be accelerated.
  */
template <FloatLike T, std::unsigned_integral K, typename series_templ>
class wynn_epsilon_3_algorithm final : public series_acceleration<T, K, series_templ>
{
private:

    const T epsilon_threshold;  ///< Threshold for epsilon correction terms to prevent division by near-zero values.
                                ///< Default: 1e-3. Smaller values may increase sensitivity but risk instability.
public:

    /**
     * @brief Parameterized constructor.
     * @param series The series object to accelerate. Must be valid and provide term and partial sum access.
     * @param epsilon_threshold_ Threshold for epsilon corrections. Controls numerical stability.
     *        Valid values: positive T values. Too small may cause overflow, too large may reduce acceleration.
     */
    explicit wynn_epsilon_3_algorithm(const series_templ& series, T epsilon_threshold_ = static_cast<T>(1e-3));

	/**
	* @brief Fast impimentation of Epsilon algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* For more information, see 612.zip 
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/

    /**
     * @brief Compute accelerated partial sum using Wynn's Epsilon Algorithm (Third Implementation).
     *
     * Implements the recursive epsilon algorithm with error control and adaptive table management.
     * The algorithm constructs a table of approximations and applies the recurrence:
     * For theory, see: Wynn (1956), Eq. (4): εₛ₊₁⁽ⁿ⁾ = εₛ₋₁⁽ⁿ⁺¹⁾ + 1/(εₛ⁽ⁿ⁺¹⁾ - εₛ⁽ⁿ⁾)
     * More information([https://calgo.acm.org/])
     * @param n The number of terms to use from the original series (partial sum index).
     *        Valid values: n > 0. Higher values use more terms but may provide better acceleration.
     * @param order The order of transformation (number of epsilon algorithm iterations).
     *        Valid values: order >= 0. Higher orders apply more transformations but may increase error.
     * @return The accelerated partial sum after applying the epsilon algorithm.
     * @throws std::domain_error if n=0.
     * @throws std::overflow_error if numerical instability (e.g., division by zero) occurs.
     */
	T operator()(K n, K order) const override;
};

// Constructor implementation
template <FloatLike T, std::unsigned_integral K, typename series_templ>
wynn_epsilon_3_algorithm<T, K, series_templ>::wynn_epsilon_3_algorithm(
    const series_templ& series,
    const T epsilon_threshold_
    ) :
    series_acceleration<T, K, series_templ>(series),
    epsilon_threshold(epsilon_threshold_)
{}

// Algorithm implementation
template <FloatLike T, std::unsigned_integral K, typename series_templ>
T wynn_epsilon_3_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::max;
    using std::abs;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");

    if (order == static_cast<K>(0)) return this->series->S_n(n);

    K N = n; // Number of terms used in transformation

    // Machine constants for numerical stability
    const T EMACH = std::numeric_limits<T>::epsilon(); ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const T EPRN = static_cast<T>(50) * EMACH;         ///< Relative error tolerance (50 * machine epsilon).
    const T OFRN = std::numeric_limits<T>::max();      ///< Overflow threshold (largest finite value).

    T result = static_cast<T>(0);       ///< Current best accelerated estimate.
    T abs_error = static_cast<T>(0);    ///< Absolute error estimate for current result.
    T resla = static_cast<T>(0);        ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    T RES, E0, E1, E2, E3, DELTA1, DELTA2, DELTA3, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS, EPSINF; // int -> K

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<T> e(
        N + static_cast<K>(3),
        static_cast<T>(0)
    ); //First N eliments of epsilon table + 2 elements for math

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = this->series->S_n(i);

    // Apply epsilon algorithm for 'order' iterations
    for (K i = static_cast<K>(0); i <= order; ++i) { //Working with Epsilon Table order times

        num = NUM = K1 = N = n;
        K NEWELM = newelm = (N - static_cast<K>(1)) / static_cast<K>(2);    // Number of new elements to compute
        e[N + static_cast<K>(2)] = e[N];                                    // Guard element for boundary
        e[N] = abs_error = OFRN;                                            // Initialize error to large value

        // Process each new element in the current diagonal
        for (K I = static_cast<K>(1); I <= NEWELM; ++I) { //Counting all diagonal elements of epsilon table

            // For theory, see: Wynn (1956), Eq. (4): εₛ₊₁⁽ⁿ⁾ = εₛ₋₁⁽ⁿ⁺¹⁾ + 1/(εₛ⁽ⁿ⁺¹⁾ - εₛ⁽ⁿ⁾)
            RES = e[K1 + static_cast<K>(2)];    // εₛ⁽ⁿ⁺¹⁾
            E0 = e[K1 - static_cast<K>(2)];     // εₛ₋₂⁽ⁿ⁾
            E1 = e[K1 - static_cast<K>(1)];     // εₛ₋₁⁽ⁿ⁾
            E2 = RES;                           // εₛ⁽ⁿ⁺¹⁾

            DELTA2 = E2 - E1;                   // εₛ⁽ⁿ⁺¹⁾ - εₛ₋₁⁽ⁿ⁾

            ERR2 = abs(DELTA2);                 // Absolute difference

            TOL2 = static_cast<T>(max(          // Tolerance based on machine precision
                abs(E2), 
                abs(E1)
            ));
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = abs(DELTA3);
            TOL3 = static_cast<T>(max(
                abs(E1), 
                abs(E0)
            ));
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (ERR2 > TOL2 || ERR3 > TOL3) {

                E3 = e[K1];                     // εₛ⁽ⁿ⁾
                e[K1] = E1;                     // Store εₛ₋₁⁽ⁿ⁾ temporarily

                DELTA1 = E1 - E3;               // εₛ₋₁⁽ⁿ⁾ - εₛ⁽ⁿ⁾

                ERR1 = abs(DELTA1);

                TOL1 = static_cast<T>(max(
                    abs(E1), 
                    abs(E3)
                ));
                TOL1*= EMACH;

                // If differences are insignificant, terminate early
                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = static_cast<T>(1) / DELTA1 + static_cast<T>(1) / DELTA2 - static_cast<T>(1) / DELTA3;
                EPSINF = abs(SS * E1);

                // Check if correction term is within threshold
                if (EPSINF > epsilon_threshold) {
                    RES = E1 + static_cast<T>(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    T ERROR = ERR2 + static_cast<T>(abs(RES - E2)) + ERR3;  // Total error estimat
                    if (abs(ERROR) <= abs(abs_error)) {
                        abs_error = ERROR;
                        result = RES;                       // Update best result
                    }
                }
                else {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }
            }

            else {
                // Differences are insignificant; accept current value
                result = RES;
                abs_error = ERR2 + ERR3;
                e[K1] = result;
                break;
            }
        }

        // Adjust N to be the greatest odd number <= n if no change
        if (N == n) // making N the greatest odd number <= n
            N = (n & static_cast<K>(1)) ? n : n - static_cast<K>(1);

        // Compact the epsilon table for next iteration
        ib = (num & static_cast<K>(1)) ? static_cast<K>(1) : static_cast<K>(2);  // Start index: 1 for odd, 2 for even
        
        // Start index: 1 (odd) or 2 (even)
        ie = newelm + static_cast<K>(1);

        // Copy elements with stride 2 to compact the table
        for (K pos = ib; pos < ib + static_cast<K>(2) * ie; pos += static_cast<K>(2))
            e[pos] = e[pos + static_cast<K>(2)];

        // Shift elements if N changed
        if (num != N) {
            in = num - N + static_cast<K>(1);
            for (K j = static_cast<K>(1); j <= N; ++j, ++in)
                e[j] = e[in];
        }

        // Update error estimate and previous result
        abs_error = max(
            abs(result - resla), 
            abs(EPRN) * abs(result)
        );

        resla = result;
    }

    // Check for numerical instability
    if (!isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}
