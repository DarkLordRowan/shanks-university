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
  *           Must satisfy Accepted. Represents numerical precision (float, double, long double).
  *           Determines the precision of all arithmetic operations and storage.
  * @tparam K Unsigned integral type for indices, counts, and sizes.
  *           Must satisfy std::unsigned_integral. Used for indexing terms, table sizes, and loop counters.
  *           Typical types: unsigned int, std::size_t.
  * @tparam series_templ Type of the series object to accelerate. Must provide:
  *           - T operator()(K n) const: returns the n-th series term aₙ
  *           - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
  *           The series object encapsulates the sequence whose convergence is to be accelerated.
  */
template <AcceptedLike T, UnsignedIntLike K>
class wynn_epsilon_3_algorithm final : public series_acceleration<T, K>
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
    explicit wynn_epsilon_3_algorithm(const T& epsilon_threshold_ = static_cast<T>(1e-3)) : series_acceleration<T, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

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
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn must be at least " + to_string(required_size));
	}

    if (n == static_cast<K>(0)){
        throw std::domain_error("n = 0 in the input");
    }

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
    using std::max;
    using std::abs;

    K N = n; // Number of terms used in transformation

    // Machine constants for numerical stability
    const T EMACH = std::numeric_limits<T>::epsilon(); ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const T EPRN = static_cast<T>(50) * EMACH;         ///< Relative error tolerance (50 * machine epsilon).
    const T OFRN = std::numeric_limits<T>::max();      ///< Overflow threshold (largest finite value).

    T result = static_cast<T>(0.0);       ///< Current best accelerated estimate.
    T abs_error = static_cast<T>(0.0);    ///< Absolute error estimate for current data.
    T resla = static_cast<T>(0.0);        ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    T RES = static_cast<T>(0.0);
    T E0 = static_cast<T>(0.0);
    T E1 = static_cast<T>(0.0);
    T E2 = static_cast<T>(0.0);
    T E3 = static_cast<T>(0.0);
    T DELTA1 = static_cast<T>(0.0);
    T DELTA2 = static_cast<T>(0.0);
    T DELTA3 = static_cast<T>(0.0);
    T ERR1 = static_cast<T>(0.0);
    T ERR2 = static_cast<T>(0.0);
    T ERR3 = static_cast<T>(0.0); 
    T TOL1 = static_cast<T>(0.0);
    T TOL2 = static_cast<T>(0.0);
    T TOL3 = static_cast<T>(0.0); 
    T SS = static_cast<T>(0.0);

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<T> e(
        N + static_cast<K>(3),
        static_cast<T>(0.0)  
    ); //First N eliments of epsilon table + 2 elements for math

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = data.Sn.at(i);

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

            TOL2 = max(          // Tolerance based on machine precision
                abs(E2), 
                abs(E1)
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = abs(DELTA3);
            TOL3 = max(
                abs(E1), 
                abs(E0)
            );
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (abs(ERR2) > TOL2 || abs(ERR3) > TOL3) {

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
                if (abs(ERR1) <= TOL1 || abs(ERR2) <= TOL2 || abs(ERR3) <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = static_cast<T>(1) / DELTA1 + static_cast<T>(1) / DELTA2 - static_cast<T>(1) / DELTA3;

                // Check if correction term is within threshold
                if (abs(SS * E1) > epsilon_threshold) {
                    RES = E1 + static_cast<T>(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    T ERROR = ERR2 + abs(RES - E2) + ERR3;  // Total error estimat
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
    if(!isfinite(result)){
        throw std::overflow_error("division by zero");
    }

    return result;

}

#ifdef INC_FPRECISION

template <UnsignedIntLike K>
class wynn_epsilon_3_algorithm<float_precision, K> final : public series_acceleration<float_precision, K>
{
private:

    const float_precision epsilon_threshold;  ///< Threshold for epsilon correction terms to prevent division by near-zero values.
                                ///< Default: 1e-3. Smaller values may increase sensitivity but risk instability.
public:

    /**
     * @brief Parameterized constructor.
     * @param series The series object to accelerate. Must be valid and provide term and partial sum access.
     * @param epsilon_threshold_ Threshold for epsilon corrections. Controls numerical stability.
     *        Valid values: positive T values. Too small may cause overflow, too large may reduce acceleration.
     */
    explicit wynn_epsilon_3_algorithm(const float_precision& epsilon_threshold_ = float_precision(1e-3)) : series_acceleration<float_precision, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

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
	float_precision operator()(
        const K n, 
        const K order, 
        const series_result<float_precision>& data
    ) const override;
};


// Algorithm implementation
template <UnsignedIntLike K>
float_precision wynn_epsilon_3_algorithm<float_precision, K>::operator()(
    const K n, 
    const K order, 
    const series_result<float_precision>& data
) const {

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn must be at least " + to_string(required_size));
	}

    if (n == static_cast<K>(0)){
        throw std::domain_error("n = 0 in the input");
    }

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
    using std::max;
    using std::abs;

    const size_t precision = data.Sn[0].precision();

    K N = n; // Number of terms used in transformation

    // Machine constants for numerical stability
    const float_precision EMACH = std::numeric_limits<float_precision>::epsilon(); ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const float_precision EPRN = static_cast<float_precision>(50) * EMACH;         ///< Relative error tolerance (50 * machine epsilon).
    const float_precision OFRN = std::numeric_limits<float_precision>::max();      ///< Overflow threshold (largest finite value).

    float_precision result = float_precision(0, precision);      ///< Current best accelerated estimate.
    float_precision abs_error = float_precision(0, precision);   ///< Absolute error estimate for current data.
    float_precision resla = float_precision(0, precision);       ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    float_precision RES = float_precision(0, precision); ;
    float_precision E0  = float_precision(0, precision); ;
    float_precision E1  = float_precision(0, precision); ;
    float_precision E2  = float_precision(0, precision); ;
    float_precision E3  = float_precision(0, precision); ;
    float_precision DELTA1 = float_precision(0, precision); ;
    float_precision DELTA2 = float_precision(0, precision); ;
    float_precision DELTA3 = float_precision(0, precision); ;
    float_precision ERR1 = float_precision(0, precision); ;
    float_precision ERR2 = float_precision(0, precision); ;
    float_precision ERR3 = float_precision(0, precision); ; 
    float_precision TOL1 = float_precision(0, precision); ;
    float_precision TOL2 = float_precision(0, precision); ;
    float_precision TOL3 = float_precision(0, precision); ; 
    float_precision SS   = float_precision(0, precision); ;

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<float_precision> e(
        N + static_cast<K>(3),
        float_precision(0, precision) 
    ); //First N eliments of epsilon table + 2 elements for math

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = data.Sn.at(i);

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

            TOL2 = max(          // Tolerance based on machine precision
                abs(E2), 
                abs(E1)
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = abs(DELTA3);
            TOL3 = max(
                abs(E1), 
                abs(E0)
            );
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (abs(ERR2) > TOL2 || abs(ERR3) > TOL3) {

                E3 = e[K1];                     // εₛ⁽ⁿ⁾
                e[K1] = E1;                     // Store εₛ₋₁⁽ⁿ⁾ temporarily

                DELTA1 = E1 - E3;               // εₛ₋₁⁽ⁿ⁾ - εₛ⁽ⁿ⁾

                ERR1 = abs(DELTA1);

                TOL1 = max(
                    abs(E1), 
                    abs(E3)
                );
                TOL1*= EMACH;

                // If differences are insignificant, terminate early
                if (abs(ERR1) <= TOL1 || abs(ERR2) <= TOL2 || abs(ERR3) <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = float_precision(1) / DELTA1 + float_precision(1) / DELTA2 - float_precision(1) / DELTA3;

                // Check if correction term is within threshold
                if (abs(SS * E1) > epsilon_threshold) {
                    RES = E1 + float_precision(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    float_precision ERROR = ERR2 + abs(RES - E2) + ERR3;  // Total error estimat
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
    if(!isfinite(result)){
        throw std::overflow_error("division by zero");
    }

    return result;

}

#ifdef INC_COMPLEXPRECISION

template <std::floating_point T, UnsignedIntLike K>
class wynn_epsilon_3_algorithm<complex_precision<T>, K> final : public series_acceleration<complex_precision<T>, K>
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
    explicit wynn_epsilon_3_algorithm(const T& epsilon_threshold_ = T(1e-3)) : series_acceleration<complex_precision<T>, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

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
	complex_precision<T> operator()(
        const K n, 
        const K order, 
        const series_result<complex_precision<T>>& data
    ) const override;
};

// Algorithm implementation
template <std::floating_point T, UnsignedIntLike K>
complex_precision<T> wynn_epsilon_3_algorithm<complex_precision<T>, K>::operator()(
    const K n, 
    const K order, 
    const series_result<complex_precision<T>>& data
) const {

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn must be at least " + to_string(required_size));
	}

    if (n == static_cast<K>(0)){
        throw std::domain_error("n = 0 in the input");
    }

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
    using std::max;
    using std::abs;

    K N = n; // Number of terms used in transformation

    // Machine constants for numerical stability
    const T EMACH = std::numeric_limits<T>::epsilon(); ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const T EPRN = static_cast<T>(50) * EMACH;         ///< Relative error tolerance (50 * machine epsilon).
    const T OFRN = std::numeric_limits<T>::max();      ///< Overflow threshold (largest finite value).

    complex_precision<T> result = static_cast<T>(0.0);       ///< Current best accelerated estimate.
    T abs_error = static_cast<T>(0.0);    ///< Absolute error estimate for current data.
    complex_precision<T> resla = complex_precision<T>(0.0);        ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    complex_precision<T> RES    = complex_precision<T>(0.0);
    complex_precision<T> E0     = complex_precision<T>(0.0);
    complex_precision<T> E1     = complex_precision<T>(0.0);
    complex_precision<T> E2     = complex_precision<T>(0.0);
    complex_precision<T> E3     = complex_precision<T>(0.0);
    complex_precision<T> DELTA1 = complex_precision<T>(0.0);
    complex_precision<T> DELTA2 = complex_precision<T>(0.0);
    complex_precision<T> DELTA3 = complex_precision<T>(0.0);

    T ERR1 = static_cast<T>(0.0);
    T ERR2 = static_cast<T>(0.0);
    T ERR3 = static_cast<T>(0.0); 
    T TOL1 = static_cast<T>(0.0);
    T TOL2 = static_cast<T>(0.0);
    T TOL3 = static_cast<T>(0.0); 
    complex_precision<T> SS = static_cast<T>(0.0);

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<complex_precision<T>> e(
        N + static_cast<K>(3),
        complex_precision<T>(0)
    ); //First N eliments of epsilon table + 2 elements for math

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = data.Sn.at(i);

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

            TOL2 = max(          // Tolerance based on machine precision
                abs(E2), 
                abs(E1)
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = abs(DELTA3);
            TOL3 = max(
                abs(E1), 
                abs(E0)
            );
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (abs(ERR2) > TOL2 || abs(ERR3) > TOL3) {

                E3 = e[K1];                     // εₛ⁽ⁿ⁾
                e[K1] = E1;                     // Store εₛ₋₁⁽ⁿ⁾ temporarily

                DELTA1 = E1 - E3;               // εₛ₋₁⁽ⁿ⁾ - εₛ⁽ⁿ⁾

                ERR1 = abs(DELTA1);

                TOL1 = max(
                    abs(E1), 
                    abs(E3)
                );
                TOL1*= EMACH;

                // If differences are insignificant, terminate early
                if (abs(ERR1) <= TOL1 || abs(ERR2) <= TOL2 || abs(ERR3) <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = complex_precision<T>(1) / DELTA1 + complex_precision<T>(1) / DELTA2 - complex_precision<T>(1) / DELTA3;

                // Check if correction term is within threshold
                if (abs(SS * E1) > epsilon_threshold) {
                    RES = E1 + complex_precision<T>(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    T ERROR = ERR2 + abs(RES - E2) + ERR3;  // Total error estimat
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
    if(!isfinite(result.real()) || !isfinite(result.imag())){
        throw std::overflow_error("division by zero");
    }

    return result;

}

template <UnsignedIntLike K>
class wynn_epsilon_3_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K>
{
private:

    const float_precision epsilon_threshold;  ///< Threshold for epsilon correction terms to prevent division by near-zero values.
                                ///< Default: 1e-3. Smaller values may increase sensitivity but risk instability.
public:

    /**
     * @brief Parameterized constructor.
     * @param series The series object to accelerate. Must be valid and provide term and partial sum access.
     * @param epsilon_threshold_ Threshold for epsilon corrections. Controls numerical stability.
     *        Valid values: positive T values. Too small may cause overflow, too large may reduce acceleration.
     */
    explicit wynn_epsilon_3_algorithm(const float_precision& epsilon_threshold_ = float_precision(1e-3)) : 
    series_acceleration<complex_precision<float_precision>, K>("wynn epsilon 3"), epsilon_threshold(epsilon_threshold_) {};

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
	complex_precision<float_precision> operator()(
        const K n, 
        const K order, 
        const series_result<complex_precision<float_precision>>& data
    ) const override;
};

// Algorithm implementation
template <UnsignedIntLike K>
complex_precision<float_precision> wynn_epsilon_3_algorithm<complex_precision<float_precision>, K>::operator()(
    const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const {

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size){
        throw std::out_of_range("The Sn smaller then required for wynn_epsilon_3_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn must be at least " + to_string(required_size));
	}

    if (n == static_cast<K>(0)){
        throw std::domain_error("n = 0 in the input");
    }

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
    using std::max;
    using std::abs;

    const size_t precision = std::max(data.Sn[0].real().precision(), data.Sn[0].imag().precision());

    K N = n; // Number of terms used in transformation

    // Machine constants for numerical stability
    const float_precision EMACH = std::numeric_limits<float_precision>::epsilon(); ///< Machine epsilon: smallest number such that 1.0 + ε ≠ 1.0.
    const float_precision EPRN = float_precision(50) * EMACH;         ///< Relative error tolerance (50 * machine epsilon).
    const float_precision OFRN = std::numeric_limits<float_precision>::max();      ///< Overflow threshold (largest finite value).

    complex_precision<float_precision> result = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );       ///< Current best accelerated estimate.
    float_precision abs_error = float_precision(0.0 ,precision);    ///< Absolute error estimate for current data.
    complex_precision<float_precision> resla = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );        ///< Previous result for error comparison.
    K newelm, num, NUM, K1, ib, ie, in; // Loop indices and counters.
    complex_precision<float_precision> RES    = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> E0     = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> E1     = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> E2     = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> E3     = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> DELTA1 = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> DELTA2 = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );
    complex_precision<float_precision> DELTA3 = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );

    float_precision ERR1 = float_precision(0.0 ,precision);
    float_precision ERR2 = float_precision(0.0 ,precision);
    float_precision ERR3 = float_precision(0.0 ,precision);
    float_precision TOL1 = float_precision(0.0 ,precision);
    float_precision TOL2 = float_precision(0.0 ,precision);
    float_precision TOL3 = float_precision(0.0 ,precision);
    complex_precision<float_precision> SS = complex_precision<float_precision>(
        float_precision(0.0 ,precision),
        float_precision(0.0 ,precision)
    );

    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    // For theory, see: Wynn (1956), Section 3: Algorithm and lozenge diagram.
    // The epsilon table e[0..N+2] stores intermediate values εₛ⁽ⁿ⁾.
    std::vector<complex_precision<float_precision>> e(
        N + static_cast<K>(3),
        complex_precision<float_precision>(
            float_precision(0.0 ,precision),
            float_precision(0.0 ,precision)
        )
    ); //First N eliments of epsilon table + 2 elements for math

    // Initialize epsilon table with partial sums: ε₀⁽ⁱ⁾ = S_i for i=0,...,N
    for (K i = static_cast<K>(0); i <= N; ++i) //Filling up Epsilon Table
        e[i] = data.Sn.at(i);

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

            TOL2 = max(          // Tolerance based on machine precision
                abs(E2), 
                abs(E1)
            );
            TOL2*=EMACH;

            DELTA3 = E1 - E0;                   // εₛ₋₁⁽ⁿ⁾ - εₛ₋₂⁽ⁿ⁾
            ERR3 = abs(DELTA3);
            TOL3 = max(
                abs(E1), 
                abs(E0)
            );
            TOL3*= EMACH;

            // Check if differences are significant relative to tolerances
            if (abs(ERR2) > TOL2 || abs(ERR3) > TOL3) {

                E3 = e[K1];                     // εₛ⁽ⁿ⁾
                e[K1] = E1;                     // Store εₛ₋₁⁽ⁿ⁾ temporarily

                DELTA1 = E1 - E3;               // εₛ₋₁⁽ⁿ⁾ - εₛ⁽ⁿ⁾

                ERR1 = abs(DELTA1);

                TOL1 = max(
                    abs(E1), 
                    abs(E3)
                );
                TOL1*= EMACH;

                // If differences are insignificant, terminate early
                if (abs(ERR1) <= TOL1 || abs(ERR2) <= TOL2 || abs(ERR3) <= TOL3) {
                    N = static_cast<K>(2) * I - static_cast<K>(1);
                    break;
                }

                // For theory, see: Wynn (1962), Eq. (13): Rational function extrapolation step.
                SS = complex_precision<float_precision>(1) / DELTA1 + complex_precision<float_precision>(1) / DELTA2 - complex_precision<float_precision>(1) / DELTA3;

                // Check if correction term is within threshold
                if (abs(SS * E1) > epsilon_threshold) {
                    RES = E1 + complex_precision<float_precision>(1) / SS;      // Apply epsilon correction
                    e[K1] = RES;                            // Store updated value
                    K1 -= static_cast<K>(2);                // Move to previous position in table
                    float_precision ERROR = ERR2 + abs(RES - E2) + ERR3;  // Total error estimat
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
    if(!isfinite(result.real()) || !isfinite(result.imag())){
        throw std::overflow_error("division by zero");
    }

    return result;

}

#endif
#endif