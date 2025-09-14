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
 * @tparam T Floating-point type for series elements (must satisfy FloatLike)
 *           Represents numerical precision (float, double, long double).
 *           Used for all numerical computations and storage.
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and loop control.
 *           Valid values: K >= 0, typically size_t or unsigned int.
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 *           The series object encapsulates the mathematical sequence to be accelerated.
 */
template <FloatLike T, std::unsigned_integral K, typename series_templ>
class ford_sidi_3_algorithm final : public series_acceleration<T, K, series_templ>{
public:

    /**
    * @brief Parameterized constructor to initialize the Ford-Sidi V-3 Algorithm.
    * @param series The series class object to be accelerated.
    *        Must be a valid object implementing the required series interface.
    *        The series object should provide access to terms and partial sums.
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
     * @param n The number of terms to use in the transformation (starting index).
     *        Valid values: n > 0 (algorithm requires at least 1 term).
     *        Represents the starting index for the transformation process.
     * @param order The order of transformation (unused in this implementation,
     *        maintained for interface consistency with base class).
     *        Valid values: order >= 0 (typically set to 0 or ignored).
     * @return The accelerated partial sum after Ford-Sidi transformation.
     * @throws std::domain_error if n=0 is provided as input.
     * @throws std::overflow_error if division by zero or numerical instability occurs.
     */
    T operator()(K n, K k) const override;
};

template <FloatLike T, std::unsigned_integral K, typename series_templ>
ford_sidi_3_algorithm<T, K, series_templ>::ford_sidi_3_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <FloatLike T, std::unsigned_integral K, typename series_templ>
T ford_sidi_3_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;

    // For theory, see: Ford & Sidi (1987), Section 1 - Input validation
    // The algorithm requires at least one term for meaningful computation
    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");
    
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения

    // For theory, see: Osada (2000), Section 4 - Efficient implementation
    // Algorithm uses auxiliary sequences for improved computational efficiency
    const K n1 = n - static_cast<K>(1);
    const K m  = n + static_cast<K>(1);

    // For theory, see: Ford & Sidi (1987), Section 2 - Auxiliary sequence initialization
    // G sequence: Used for storing transformation coefficients
    std::vector<T> G(m + static_cast<K>(1));

    // FSA sequence: Stores accelerated partial sums
    std::vector<T> FSA(G.size());

    // FSI sequence: Stores normalization factors
    std::vector<T> FSI(G.size());

    // FSG matrix: Stores intermediate transformation values
    std::vector<std::vector<T>> FSG(
        m + static_cast<K>(2), 
        std::vector<T>(G.size())
    );

    // For theory, see: Osada (2000), Eq. (9) - Initial coefficient computation
    // G[0] = a_{n-1} * n, where a_{n-1} is the (n-1)-th series term
    G[0] = this->series->operator()(n1) * static_cast<T>(n);

    // For theory, see: Ford & Sidi (1987), Eq. (2.3) - Recursive coefficient scaling
    // Te = 1/n used for recursive computation of G sequence
    T Te = static_cast<T>(1) / static_cast<T>(n);

    // For theory, see: Osada (2000), Section 2.2 - Recursive G sequence computation
    // G[k] = (1/n) * G[k-1] for k = 1, 2, ..., m
    for (K k = static_cast<K>(1); k <= m; ++k)
        G[k] = Te * G[k - static_cast<K>(1)];

    // For theory, see: Osada (2000), Section 4 - Initialization of transformation sequences
    // FSA[n1] = S_{n-1} (partial sum up to term n-1)
    FSA[n1] = this->series->S_n(n1);

    // FSI[n1] = 1 (initial normalization factor)
    FSI[n1] = static_cast<T>(1);

    if (G[0] != static_cast<T>(0)) {
        // For theory, see: Ford & Sidi (1987), Eq. (2.4) - Normalization when G[0] ≠ 0
        // Normalize sequences by dividing by G[0]
        FSA[n1] /= G[0];
        FSI[n1] /= G[0];
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i] / G[0];
    } 
    else {
        // For theory, see: Ford & Sidi (1987), Section 3 - Alternative initialization
        // When G[0] = 0, use direct values without normalization
        for (K i = static_cast<K>(1); i <= m; ++i)
            FSG[i][n1] = G[i];
    }
    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения

    // For theory, see: Osada (2000), Section 4 - Main algorithm loop
    // Main Ford-Sidi transformation computation
    K MM, MM1, k2;
    T D;

    // Основной цикл алгоритма Ford-Sidi
    for (K k = static_cast<K>(0); k <= n1; ++k) {
        MM = n1 - k;
        MM1 = MM + static_cast<K>(1);
        k2 = k + static_cast<K>(2);

        // For theory, see: Ford & Sidi (1987), Eq. (2.8) - Denominator computation
        // D = FSG[k2][MM1] - FSG[k2][MM] (difference for normalization)
        D = FSG[k2][MM1] - FSG[k2][MM];

        // For theory, see: Osada (2000), Section 4 - FSG sequence update
        // Update FSG values using difference operations: FSG[i][MM] = (FSG[i][MM1] - FSG[i][MM]) / D
        for (K i = k + static_cast<K>(3); i <= m; ++i){
            FSG[i][MM] = FSG[i][MM1] - FSG[i][MM];
            FSG[i][MM]/= D;
        }

        // For theory, see: Osada (2000), Eq. (20) - FSA sequence update
        // FSA[MM] = (FSA[MM1] - FSA[MM]) / D (accelerated sum update)
        FSA[MM] = FSA[MM1] - FSA[MM];
        FSA[MM]/= D;

        // For theory, see: Osada (2000), Section 4 - FSI sequence update  
        // FSI[MM] = (FSI[MM1] - FSI[MM]) / D (normalization factor update)
        FSI[MM] = FSI[MM1] - FSI[MM];
        FSI[MM]/= D;
    }

    // For theory, see: Osada (2000), Eq. (11) - Final result computation
    // T = FSA[0] / FSI[0] (final accelerated sum)
    FSA[0] /= FSI[0];

    // For theory, see: Ford & Sidi (1987), Section 3 - Numerical stability check
    // Ensure the result is a finite floating-point value
    if (!isfinite(FSA[0]))
        throw std::overflow_error("division by zero");

    return FSA[0];
}