/**
 * @file ford_sidi_3_algorithm.hpp
 * @brief This file contains the declaration of the Ford-Sidi Algorithm class.
 *        This implementation is based on the efficient Ford-Sidi algorithm
 *        that requires fewer arithmetic operations than the E-algorithm.
 */

 // For theory, see: 
 // Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
 // Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.

#pragma once

#include "../series_acceleration.hpp"
#include <vector> // Include the vector library
#include <cmath>  // Include for isfinite

/**
 * @brief Ford-Sidi algorithm class template implementing an efficient extrapolation method.
 *
 * @authors Sharonov K.S.
 *
 * This algorithm provides a more economical implementation compared to the standard E-algorithm
 * while maintaining mathematical equivalence. It is particularly effective for sequences
 * with specific convergence patterns.
 *
 * References:
 * - Ford, W.F., Sidi, A. (1987). An algorithm for a generalization of the Richardson extrapolation process.
 *   SIAM Journal on Numerical Analysis, 24(5), 1212-1232.
 * - Osada, N. (2000). The E-algorithm and the Ford-Sidi algorithm.
 *   Journal of Computational and Applied Mathematics, 122(1), 223-230.
 *
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class ford_sidi_3_algorithm final : public series_acceleration<T, K, series_templ>{
public:

    /**
     * @brief Parameterized constructor to initialize the Ford-Sidi V-3 Algorithm.
     * @param series The series class object to be accelerated
     *        Must be a valid object implementing the required series interface
     */
    explicit ford_sidi_3_algorithm(const series_templ& series);

    /**
     * @brief Fast implementation of Ford-Sidi algorithm for series acceleration.
     *
     * Computes the accelerated sum using the efficient Ford-Sidi implementation
     * that requires fewer arithmetic operations than the standard E-algorithm.
     * The algorithm uses auxiliary sequences and difference operations.
     *
     * For theory, see: Osada (2000), Section 4, Eq. (20)
     * T_k^{(n)} = [ψ_{k-1}^{(n+1)}(s) - ψ_{k-1}^{(n)}(s)] / [ψ_{k-1}^{(n+1)}(1) - ψ_{k-1}^{(n)}(1)]
     *
     * @param n The number of terms to use in the transformation
     *        Valid values: n > 0 (algorithm requires at least 1 term)
     *        Higher values use more terms but may provide better acceleration
     * @param order The order of transformation (unused in this implementation,
     *        maintained for interface consistency)
     *        Valid values: order >= 0 (typically ignored)
     * @return The accelerated partial sum after Ford-Sidi transformation
     * @throws std::domain_error if n=0 is provided as input
     * @throws std::overflow_error if division by zero or numerical instability occurs
     */
    T operator()(K n, K k) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
ford_sidi_3_algorithm<T, K, series_templ>::ford_sidi_3_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T ford_sidi_3_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");
    
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения

    // For theory, see: Osada (2000), Section 4
    // Efficient implementation of Ford-Sidi algorithm using auxiliary sequences
    const K n1 = n - static_cast<K>(1);
    const K m  = n + static_cast<K>(1);

    // For theory, see: Ford & Sidi (1987), Section 2
    // Initialization of auxiliary sequences G, FSA, FSI, and FSG
    std::vector<T> G(m + static_cast<K>(1));
    std::vector<T> FSA(G.size());
    std::vector<T> FSI(G.size());
    std::vector<std::vector<T>> FSG(
        m + static_cast<K>(2), 
        std::vector<T>(G.size())
    );

    // For theory, see: Osada (2000), Eq. (9)
    // Initialization of G sequence: G[0] = a_{n-1} * n
    G[0] = this->series->operator()(n1) * static_cast<T>(n);
    
    T Te = static_cast<T>(1) / static_cast<T>(n);

    // For theory, see: Osada (2000), Section 2.2
    // Recursive computation of G sequence
    for (K k = static_cast<K>(1); k <= m; ++k)
        G[k] = Te * G[k - static_cast<K>(1)];

    // For theory, see: Osada (2000), Section 4
    // Initialization of FSA and FSI sequences
    FSA[n1] = this->series->S_n(n1);
    FSI[n1] = static_cast<T>(1);

    if (G[0] != static_cast<T>(0)) {
        // For theory, see: Ford & Sidi (1987), Eq. (2.4)
        // Normalization of sequences when G[0] is nonzero
        FSA[n1] /= G[0];
        FSI[n1] /= G[0];
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i] / G[0];
    } 
    else {
        // For theory, see: Ford & Sidi (1987), Section 3
        // Alternative initialization when G[0] is zero
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i];
    }
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения

    // Main Ford-Sidi algorithm loop
    // For theory, see: Osada (2000), Section 4, Algorithm description
    K MM, MM1, k2;
    T D;

    // Основной цикл алгоритма Ford-Sidi
    for (K k = static_cast<K>(0); k <= n1; ++k) {
        MM = n1 - k;
        MM1 = MM + static_cast<K>(1);
        k2 = k + static_cast<K>(2);

        // For theory, see: Ford & Sidi (1987), Eq. (2.8)
        // Computation of denominator D = FSG[k2][MM1] - FSG[k2][MM]
        D = FSG[k2][MM1] - FSG[k2][MM];

        // For theory, see: Osada (2000), Section 4, Recursive relations
        // Update of FSG sequences using difference operations
        for (K i = k + static_cast<K>(3); i <= m; ++i){
            FSG[i][MM] = FSG[i][MM1] - FSG[i][MM];
            FSG[i][MM]/= D;
        }

        // For theory, see: Osada (2000), Eq. (20)
        // Update of FSA sequence: FSA[MM] = (FSA[MM1] - FSA[MM]) / D
        FSA[MM] = FSA[MM1] - FSA[MM];
        FSA[MM]/= D;

        // For theory, see: Osada (2000), Section 4
        // Update of FSI sequence: FSI[MM] = (FSI[MM1] - FSI[MM]) / D
        FSI[MM] = FSI[MM1] - FSI[MM];
        FSI[MM]/= D;
    }

    // For theory, see: Osada (2000), Eq. (11)
    // Final result computation: T = FSA[0] / FSI[0]
    FSA[0] /= FSI[0]; //result 

    if (!isfinite(FSA[0]))
        throw std::overflow_error("division by zero");

    return FSA[0];
}