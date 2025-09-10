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
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 *           Used for all mathematical computations and storage of series terms
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class brezinski_theta_algorithm final : public series_acceleration<T, K, series_templ>
{
protected:

    /**
     * @brief Recursive function to compute theta transformation values.
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
    T theta(K n, K order, T S_n, K j) const;

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
    T operator()(K n, K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::theta(K n, const K order, T S_n, const K j) const {

    using std::isfinite;
    using std::fma;

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.12)
    // Base case for order = 1: θ₁⁽ⁿ⁾ = 1/a_{n+j+1}
    if (order == static_cast<K>(1)) {

        T res = static_cast<T>(1) / this->series->operator()(n + j + static_cast<K>(1));
        if (!isfinite(res))
            throw std::overflow_error("division by zero");
        return res;
    }

    // For theory, see: Brezinski (2003), Section 3.2, Eq. (3.15)
    // Compute partial sum from n+1 to n+j: S_{n+j} = S_n + Σ_{k=n+1}^{n+j} a_k
    for (K tmp = n + static_cast<K>(1); tmp <= n + j; ++tmp)
        S_n += this->series->operator()(tmp);

    n += j;

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base case for order = 0: θ₀⁽ⁿ⁾ = S_n (partial sum)
    if (order == static_cast<K>(0))
        return S_n;

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого 
    // Precompute orders for recursive calls
    const K order1 = order - static_cast<K>(1);
    const K order2 = order - static_cast<K>(2);

    // For theory, see: Brezinski (2003), Section 10.2, Eq. (10.2.4)
    // Recursive computation of theta values with different offsets
    const T theta_order1_0 = theta(n, order1, S_n, static_cast<K>(0));
    const T theta_order1_1 = theta(n, order1, S_n, static_cast<K>(1));
    const T theta_order1_2 = theta(n, order1, S_n, static_cast<K>(2));
    const T theta_order2_1 = theta(n, order2, S_n, static_cast<K>(1));

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.13a)
    // Odd order transformation: θ₂ₖ₊₁⁽ⁿ⁾ = θ₂ₖ₋₁⁽ⁿ⁺¹⁾ + 1/(θ₂ₖ⁽ⁿ⁾ - θ₂ₖ⁽ⁿ⁺¹⁾)
    if (order & static_cast<K>(1)) { // order is odd
        const T delta = static_cast<T>(1) / (theta_order1_0 - theta_order1_1); // 1/Δθ₂ₖ⁽ⁿ⁾
        if (!isfinite(delta))
            throw std::overflow_error("division by zero");
        return theta_order2_1 + delta; // θ₂ₖ₊₁⁽ⁿ⁾ = θ₂ₖ₋₁⁽ⁿ⁺¹⁾ + 1/Δθ₂ₖ⁽ⁿ⁾
    }

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.13b)
    // Even order transformation: θ₂ₖ₊₂⁽ⁿ⁾ = θ₂ₖ⁽ⁿ⁺¹⁾ + (Δθ₂ₖ⁽ⁿ⁺¹⁾·Δθ₂ₖ₊₁⁽ⁿ⁺¹⁾)/Δ²θ₂ₖ₊₁⁽ⁿ⁾
    const T delta2 = static_cast<T>(1) / fma(static_cast<T>(-2), theta_order1_1, theta_order1_0 + theta_order1_2); // 1/Δ²θ₂ₖ₊₁⁽ⁿ⁾

    if (!isfinite(delta2))
        throw std::overflow_error("division by zero");

    const T delta_n = theta_order2_1 - theta(n, order2, S_n, static_cast<K>(2)); // Δθ₂ₖ⁽ⁿ⁺¹⁾
    const T delta_n1 = theta_order1_1 - theta_order1_2; // Δθ₂ₖ₊₁⁽ⁿ⁺¹⁾

    return fma(delta_n * delta_n1, delta2, theta_order2_1); // θ₂ₖ₊₂⁽ⁿ⁾ = θ₂ₖ⁽ⁿ⁺¹⁾ + (Δθ₂ₖ⁽ⁿ⁺¹⁾·Δθ₂ₖ₊₁⁽ⁿ⁺¹⁾)/Δ²θ₂ₖ₊₁⁽ⁿ⁾
}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T brezinski_theta_algorithm<T, K, series_templ>::operator()(const K n, const K order) const{

    // For theory, see: Brezinski (2003), Section 10.2, Theorem 10.2.1
    // Only even orders have mathematical meaning in the final result
    if (order & static_cast<K>(1)) // is order odd?
        throw std::domain_error("order should be even number");

    // For theory, see: Brezinski (1977), Chapter 4, Eq. (4.10)
    // Base cases: return partial sum for n=0 or order=0
    if (n == static_cast<K>(0) || order == static_cast<K>(0))
        return this->series->S_n(n);

    // For theory, see: Brezinski (2003), Section 10.2, Eq. (10.2.4)
    // Start recursive computation with initial parameters
    return theta(n, order, this->series->S_n(n), static_cast<K>(0));
}
