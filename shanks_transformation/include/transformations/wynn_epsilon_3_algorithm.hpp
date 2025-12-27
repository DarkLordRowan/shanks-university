#ifndef WYNN_EPSILON_3_ALGORITHM_HPP
#define WYNN_EPSILON_3_ALGORITHM_HPP
#pragma once
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

#include "series_acceleration.hpp"

 /**
  * @brief Wynn's Epsilon Algorithm (Third Implementation) class template.
  *
  * This implementation combines numerical stability enhancements with efficient memory usage.
  * It computes accelerated partial sums using the epsilon algorithm with threshold-based
  * error control and adaptive table management.
  *
  * Template Parameters:
  * @tparam T Floating-point type for series elements and computations.
  *           Must satisfy Accepted. Represents numerical precision (float, double, long double).
  *           Determines the precision of all arithmetic operations and storage.
  * @tparam K Unsigned integral type for indices, counts, and sizes.
  *           Must satisfy std::unsigned_integral. Used for indexing terms, table sizes, and loop counters.
  *           Typical types: unsigned int, std::size_t.
  */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_3_algorithm final : public series_acceleration<T, K>
{
public:

    using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

    /**
     * @brief Parameterized constructor.
     * @param epsilon_threshold_ Threshold for epsilon corrections. Controls numerical stability.
     *        Valid values: positive T values. Too small may cause overflow, too large may reduce acceleration.
     */
    explicit wynn_epsilon_3_algorithm(const float_type& epsilon_threshold_ = utils::cast<float_type>(1e-3)) : 
    series_acceleration<T, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

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
	T operator()(
        const K n, 
        const K order, 
        const series_result<T>& data
    ) const override;
private:

    const float_type epsilon_threshold;  ///< Threshold for epsilon correction terms to prevent division by near-zero values.
                                ///< Default: 1e-3. Smaller values may increase sensitivity but risk instability.
};


// Algorithm implementation
template <AcceptedLike T, UnsignedIntLike K>
T wynn_epsilon_3_algorithm<T, K>::operator()(
    const K n, 
    const K order, 
    const series_result<T>& data
) const {

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" + utils::to_string(order) + "}^{" + utils::to_string(n) + "}\n" +
        "the size of Sn must be at least " + utils::to_string(required_size));
	}

    if (n == static_cast<K>(0)){
        throw std::domain_error("n = 0 in the input");
    }

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::max;

    K N = n; // Number of terms used in transformation

    T result = utils::cast<T>(0.0);       ///< Current best accelerated estimate.
    float_type abs_error = utils::cast<float_type>(0.0);    ///< Absolute error estimate for current data.
    T resla = utils::cast<T>(0.0);        ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    T RES = utils::cast<T>(0.0);
    T E0, E1, E2, E3;
    E0 = E1 = E2 = E3 = utils::cast<T>(0);
    T DELTA1, DELTA2, DELTA3;
    DELTA1 = DELTA2 = DELTA3 = utils::cast<T>(0);
    float_type ERROR, ERR1, ERR2, ERR3;
    ERROR = ERR1 = ERR2 = ERR3 = utils::cast<float_type>(0);
    float_type TOL1, TOL2, TOL3;
    TOL1 = TOL2 = TOL3 = utils::cast<float_type>(0);
    T SS = utils::cast<T>(0.0);

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<T> e(N + static_cast<K>(3), utils::cast<T>(0.0)); //First N eliments of epsilon table + 2 elements for math

    if constexpr (is_precisable<T>::value){
		utils::set_vec_precision(e, utils::get_precision(data.Sn[0]));
		utils::set_precision(utils::get_precision(data.Sn[0]), result, abs_error, resla, RES, E0, E1, E2, E3, 
                DELTA1, DELTA2, DELTA3, ERROR, ERR1, ERR2, ERR3, TOL1, TOL2, TOL3, SS);
    }

    // Machine constants for numerical stability
    const float_type EMACH = utils::epsilon(abs_error);                         ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const float_type EPRN = utils::cast<float_type>(50) * EMACH;                ///< Relative error tolerance (50 * machine epsilon).
    // THE 1'000'000'000 IS FOR ARB PRECISION, OTHERWISE NUMERIC LIMIT RETURNS 0
    const float_type OFRN = max(std::numeric_limits<float_type>::max(), utils::cast<float_type>(1'000'000'000)); ///< Overflow threshold (largest finite value).

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i)  e[i] += data.Sn.at(i); //Filling up Epsilon Table
    

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
            E0  = e[K1 - static_cast<K>(2)];    // εₛ₋₂⁽ⁿ⁾
            E1  = e[K1 - static_cast<K>(1)];    // εₛ₋₁⁽ⁿ⁾
            E2  = RES;                          // εₛ⁽ⁿ⁺¹⁾

            DELTA2 = E2 - E1;                   // εₛ⁽ⁿ⁺¹⁾ - εₛ₋₁⁽ⁿ⁾

            ERR2 = utils::abs(DELTA2);                 // Absolute difference
            
            TOL2 = max(utils::abs(E2), utils::abs(E1)); // Tolerance based on machine precision
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = utils::abs(DELTA3);
            TOL3 = max(utils::abs(E1), utils::abs(E0) );
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (ERR2 > TOL2 || ERR3 > TOL3) {

                E3 = e[K1];                     // εₛ⁽ⁿ⁾
                e[K1] = E1;                     // Store εₛ₋₁⁽ⁿ⁾ temporarily

                DELTA1 = E1 - E3;               // εₛ₋₁⁽ⁿ⁾ - εₛ⁽ⁿ⁾

                ERR1 = utils::abs(DELTA1);

                TOL1 = max(utils::abs(E1), utils::abs(E3));
                TOL1*= EMACH;

                // If differences are insignificant, terminate early
                if (ERR1 <= TOL1 || ERR2 <= TOL2 || ERR3 <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = utils::cast<T>(1) / DELTA1 + utils::cast<T>(1) / DELTA2 - utils::cast<T>(1) / DELTA3;
                //std::cout << "\n" << RES << "\n";
                // Check if correction term is within threshold
                if (utils::abs(SS * E1) > epsilon_threshold) {
                    RES = E1 + utils::cast<T>(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    ERROR = ERR2 + utils::abs(RES - E2) + ERR3;    // Total error estimat
                    if (ERROR <= abs_error) {
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
            N = (n % static_cast<K>(2) == static_cast<K>(1)) ? n : n - static_cast<K>(1);

        // Compact the epsilon table for next iteration
        ib = (num % static_cast<K>(2) == static_cast<K>(1) ) ? static_cast<K>(1) : static_cast<K>(2);  // Start index: 1 for odd, 2 for even
        
        // Start index: 1 (odd) or 2 (even)
        ie = newelm + static_cast<K>(1);

        // Copy elements with stride 2 to compact the table
        for (K pos = ib; pos < ib + static_cast<K>(2) * ie; pos += static_cast<K>(2)) e[pos] = e[pos + static_cast<K>(2)];

        // Shift elements if N changed
        if (num != N) {
            in = num - N + static_cast<K>(1);
            for (K j = static_cast<K>(1); j <= N; ++j, ++in)
                e[j] = e[in];
        }

        // Update error estimate and previous result
        abs_error = max(
            utils::abs(result - resla), 
            EPRN * utils::abs(result)
        );

        resla = result;
    }

    // Check for numerical instability
    if(!utils::isfinite(result)) throw std::overflow_error("division by zero");

    return result;

}

#endif