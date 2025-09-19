/**
 * @file brezinski_theta_algorithm.hpp
 * @brief This file contains the declaration of the Theta Brezinski Algorithm class,
 *        which implements Brezinski's theta algorithm for sequence transformation.
 */

 // For theory, see:
 // Brezinski, C. (1977). Acceleration de la Convergence en Analyse Numerique. Springer-Verlag.
 // Brezinski, C., & Redivo Zaglia, M. (2003). Extrapolation Methods: Theory and Practice. 
 // Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of 
 // Convergence and the Summation of Divergent Series. Computer Physics Reports, 1(1), 1-123.

#pragma once

#include "../series_acceleration.hpp"
#include <cmath> //Include for fma, isfinite
#include <vector>

/**
 * @brief Brezinski's theta algorithm class template implementing a nonlinear sequence transformation.
 *
 * @authors Yurov P.I., Bezzaborov A.A.
 *
 * This algorithm implements Brezinski's theta transformation, which is an efficient
 * method for accelerating the convergence of slowly convergent sequences and series.
 * The algorithm uses a recursive approach with specific difference operations to
 * compute transformed sequence values.
 *
 * References:
 * - Brezinski, C. (1977). Acceleration de la Convergence en Analyse Numerique.
 * - Brezinski, C., & Redivo Zaglia, M. (2003). Extrapolation Methods: Theory and Practice.
 * - Weniger, E. J. (2003). Nonlinear Sequence Transformations for the Acceleration of
 *   Convergence and the Summation of Divergent Series.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all mathematical computations and storage of series terms
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <Accepted T, std::unsigned_integral K, typename series_templ>
class brezinski_theta_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

    /**
     * @brief Function to compute theta transformation values.
     *
     * Computes the value of theta according to Brezinski's recursive formulas.
     * This function implements the core recursive structure of the theta algorithm.
     *
     * @param n The starting index for the transformation computation
     *        Valid values: n >= 0, represents the starting point in the sequence
     * @param order The order of transformation to compute
     *        Valid values: order >= 0, higher orders provide more acceleration
     * @param S_n The partial sum at index n (S_n = a_0 + ... + a_n)
     *        Used as initial value for recursive computations
     * @param j An offset parameter for recursive computations
     *        Valid values: j >= 0, used to shift the computation window
     * @return The transformed value at the specified order and parameters
     * @throws std::overflow_error if division by zero occurs during computation
     */

    inline T calculate(K n, K order) const;

public:

    /**
     * @brief Parameterized constructor to initialize the Theta Brezinski Algorithm.
     *
     * Initializes the algorithm with the provided series object. The series must
     * implement the required interface for term access and partial sum computation.
     *
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
     *        The series object is stored by reference for efficient access
     */
    explicit brezinski_theta_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

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
     * @return The accelerated partial sum after theta transformation
     * @throws std::domain_error if n=0 or order is odd
     * @throws std::overflow_error if division by zero occurs during computation
     */
    T operator()(K n, K order) override;
};


template <Accepted T, std::unsigned_integral K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::calculate(K n, const K order) const {

    using std::isfinite;

    const K base_size = static_cast<K>(3) * order / static_cast<K>(2) + static_cast<K>(1);

    std::vector<T> theta_odd(
        base_size,
        static_cast<T>(0)
    ); // vector for theta_(2n + 1)

    std::vector<T> theta_even(
        base_size,
        static_cast<T>(0)
    ); //vector for theta_(2n), in the beginning it is theta_(-1) which is zero for all i

    // init theta_(0)
    for(K j = static_cast<K>(0); j < base_size; ++j)
        theta_even[j] = this->series->Sn(n + j);


    K j1, j2;
    T delta; //temporary varaible

    for(K level = static_cast<K>(1); level <= order / static_cast<K>(2); ++level){

        // transform odd vector
        for(K j = static_cast<K>(0); j < base_size + static_cast<K>(2) - static_cast<K>(3) * level; ++j){

            j1 = j + static_cast<K>(1);
            j2 = j + static_cast<K>(2);

            delta = theta_even[j1] - theta_even[j];

            theta_odd[j] = fma(theta_odd[j1], delta, static_cast<T>(1));
            theta_odd[j]/= delta;
        }

        // transform even vector
        for(K j = static_cast<K>(0); j < base_size - static_cast<K>(3) * level; ++j){

            j1 = j + static_cast<K>(1);
            j2 = j + static_cast<K>(2);

            delta = theta_odd[j2] - theta_odd[j1];
            
            theta_even[j] = theta_even[j+1];
            theta_even[j]-= (theta_even[j+2]-theta_even[j+1]) * delta / (theta_odd[j+1] - theta_odd[j] - delta);

        }
    }

    if(!isfinite(theta_even[0]))
        throw std::overflow_error("division by zero");

    return theta_even[0];
}

template <Accepted T, std::unsigned_integral K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::operator()(const K n, const K order) {

    // For theory, see: Brezinski (2003), Section 10.2, Theorem 10.2.1
    // Only even orders have mathematical meaning in the final result
    if (order & static_cast<K>(1)) // is order odd?
        throw std::domain_error("order should be even number");

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    //if (n == static_cast<K>(0) || order == static_cast<K>(0))
    if (order == static_cast<K>(0))
        return this->series->Sn(n);

    // For theory, see: Brezinski (2003), Section 10.2, Eq. (10.2.4)
    // Start computation with initial parameters
    return calculate(n, order);
}
