/**
 * @file chang_whynn_algorithm.hpp
 * @brief This file contains the declaration of the Chang-Wynn algorithm,
 *        which combines elements of Wynn's epsilon algorithm with Chang's modifications.
 */

 // For theory, see: 
 // Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 // Chang, X.K., He, Y., Hu, X.B., Sun, J.Q., & Weniger, E.J. (2019). 
 // Construction of new generalizations of Wynn's epsilon and rho algorithm by solving 
 // finite difference equations in the transformation order. Numerical Algorithms.

#pragma once

#include "../series_acceleration.hpp"
#include <vector>   // Include the vector library
#include <cmath>    //Include for fma, isfinite

/**
 * @brief Chang-Wynn algorithm class template implementing a hybrid acceleration method.
 *
 * @authors Kreinin R.G.
 *
 * This algorithm combines Wynn's epsilon algorithm with modifications proposed by Chang
 * to improve numerical stability and convergence properties. It is particularly effective
 * for sequences with specific convergence patterns.
 *
 * References:
 * - Wynn, P. (1956). On a device for computing the eₙ(Sₙ) transformation.
 * - Chang, X.K., He, Y., Hu, X.B., Sun, J.Q., & Weniger, E.J. (2019).
 *   Construction of new generalizations of Wynn's epsilon and rho algorithm by solving
 *   finite difference equations in the transformation order. Numerical Algorithms.
 *
 * @tparam T Floating-point type for series elements (must satisfy std::floating_point)
 *           Represents numerical precision (float, double, long double)
 *           Purpose: To handle the numerical computations with desired precision.
 *           Valid values: Any standard floating-point type (float, double, long double).
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting and indexing operations.
 *           Purpose: To represent non-negative indices and transformation orders.
 *           Valid values: Any standard unsigned integral type (unsigned int, unsigned long, etc.).
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term aₙ
 *           - T S_n(K n) const: returns the n-th partial sum sₙ = a₀ + ... + aₙ
 *           Purpose: To abstract the series representation, allowing flexibility (e.g., user-defined series).
 *           Valid values: Any type meeting the above requirements.
 */
template <std::floating_point T, std::unsigned_integral K, typename series_templ>
class chang_wynn_algorithm final : public series_acceleration<T, K, series_templ>
{
public:

    /**
     * @brief Parameterized constructor to initialize the chang_wynn_algorithm.
     * @param series The series class object to be accelerated.
     *        Must be a valid object implementing the required series interface.
     *        Purpose: To provide the series data for acceleration.
     */
    explicit chang_wynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

    /**
     * @brief Implementation of Chang-Wynn hybrid algorithm for series acceleration.
     *
     * Computes the accelerated sum using a combination of Wynn's epsilon algorithm
     * and Chang's modifications. The algorithm uses a two-row approach for efficient
     * computation and includes stability checks to handle numerical issues.
     *
     * @param n The number of terms to use in the transformation.
     *        Valid values: n > 0 (algorithm requires at least 1 term).
     *        Higher values use more terms but may provide better acceleration.
     *        Purpose: Determines the number of series terms considered for acceleration.
     * @param order The order of transformation (unused in this implementation,
     *        maintained for interface consistency).
     *        Valid values: order >= 0 (typically ignored).
     *        Purpose: Reserved for future extensions; currently not utilized.
     * @return The accelerated partial sum after Chang-Wynn transformation.
     * @throws std::domain_error if n=0 is provided as input.
     * @throws std::overflow_error if division by zero or numerical instability occurs.
     */
	T operator()(K n, K order) const override;
};

template <std::floating_point T, std::unsigned_integral K, typename series_templ>
T chang_wynn_algorithm<T, K, series_templ>::operator()(const K n, const K order) const {

    using std::isfinite;
    using std::fma;

    if (n == static_cast<K>(0))
        throw std::domain_error("n = 0 in the input");

    T up, down, coef, coef2;

    //TODO спросить у Парфенова, ибо жертвуем читаемостью кода, ради его небольшого ускорения
    
    // For theory, see: Chang et al. (2019), Section 3.4, Eq. (3.20)
    // Initialization of epsilon table with modified initial conditions.
    K i1, i2, i3, k1;
    K max = n - (n & static_cast<K>(1)); // Ensure max is even for algorithm stability.

    // For theory, see: Wynn (1956), Section 3 (Algorithm)
    // Epsilon table structure: two rows to store intermediate transformations.
    std::vector<std::vector<T>> e(
        2, 
        std::vector<T>(
            n, 
            static_cast<T>(0)
        )
    ); // Two vectors of length n containing Epsilon table (current and previous rows).

    // For theory, see: Chang et al. (2019), Eq. (3.20d)
    // Vector F stores intermediate factors F₁⁽ⁿ⁾ used in the recursion.
    std::vector<T> f(n, static_cast<T>(0)); // Vector for containing F results from index 0 to n-1.

    // For theory, see: Wynn (1956), Eq. (2.6b)
    // Epsilon algorithm recursion: εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
    // Initialize the first row of the epsilon table.
    for (K i = static_cast<K>(0); i < max; ++i) {
        // For theory, see: Wynn (1956), Eq. (2.8)
        // ε₁⁽ⁿ⁾ = 1 / ΔSₙ for n >= 0.
        e[0][i] = static_cast<T>(1) / (this->series->operator()(i + static_cast<K>(1)));
    }

    // For theory, see: Chang et al. (2019), Eq. (3.20d)
    // F₁⁽ⁿ⁾ = [Δ²T₀⁽ⁿ⁾ Δ²T₀⁽ⁿ⁺¹⁾] / [ΔT₀⁽ⁿ⁺²⁾ Δ²T₀⁽ⁿ⁾ - ΔT₀⁽ⁿ⁾ Δ²T₀⁽ⁿ⁺¹⁾]
    for (K i = static_cast<K>(0); i < max; ++i) { //Counting F function

        i1 = i + static_cast<K>(1);
        i2 = i + static_cast<K>(2);
        i3 = i + static_cast<K>(3);

        // For theory, see: Chang et al. (2019), Eq. (3.20c)
        // T₂⁽ⁿ⁾ = T₀⁽ⁿ⁺¹⁾ - [ΔT₀⁽ⁿ⁾ ΔT₀⁽ⁿ⁺¹⁾ Δ²T₀⁽ⁿ⁺¹⁾] / [ΔT₀⁽ⁿ⁺²⁾ Δ²T₀⁽ⁿ⁾ - ΔT₀⁽ⁿ⁾ Δ²T₀⁽ⁿ⁺¹⁾]
        // Compute second differences: Δ²S_{n+1} = S_{n+3} - 2S_{n+2} + S_{n+1}
        coef = fma(
            static_cast<T>(-2), 
            this->series->S_n(i2), 
            this->series->S_n(i3) + this->series->S_n(i1)
        ); 

        // Compute Δ²S_n = S_{n+2} - 2S_{n+1} + S_n
        coef2 = fma(
            static_cast<T>(-2), 
            this->series->S_n(i1), 
            this->series->S_n(i2) + this->series->S_n(i)
        );

        // Numerator: ΔS_n * ΔS_{n+1} * Δ²S_{n+1}
        up = this->series->operator()(i1);
        up*= this->series->operator()(i2);
        up*= coef;

        // Denominator: ΔS_{n+2} * Δ²S_n - ΔS_n * Δ²S_{n+1}
        down = this->series->operator()(i3) * coef2;
        down -= this->series->operator()(i1) * coef;
        down = static_cast<T>(1) / down; // Reciprocal for division.
         
        // Compute T₂⁽ⁿ⁾ = S_{n+1} - (up * down)
        e[1][i] = static_cast<T>(fma(-up, down, this->series->S_n(i1)));

        // Compute F₁⁽ⁿ⁾ = Δ²S_{n+1} * Δ²S_n * down
        f[i] = coef * coef2 * down;
    }

    // For theory, see: Chang et al. (2019), Eq. (3.20e)
    // Tₖ₊₁⁽ⁿ⁾ = Tₖ₋₁⁽ⁿ⁺¹⁾ + [1 - k + k F₁⁽ⁿ⁾] / [Tₖ⁽ⁿ⁺¹⁾ - Tₖ⁽ⁿ⁾]
    for (K k = static_cast<K>(2); k <= max; ++k) {
        k1 = static_cast<K>(1) - k; // Precompute (1 - k)

        for (K i = static_cast<K>(0); i < max - k; ++i) {
            i1 = i + static_cast<K>(1);

            // Numerator: 1 - k + k * F₁⁽ⁿ⁾
            up = fma(static_cast<T>(k), f[i], static_cast<T>(k1));

            // Denominator: 1 / (Tₖ⁽ⁿ⁺¹⁾ - Tₖ⁽ⁿ⁾)
            down = static_cast<T>(1);
            down/= (e[1][i1] - e[1][i]);

            // Tₖ₊₁⁽ⁿ⁾ = Tₖ₋₁⁽ⁿ⁺¹⁾ + (up * down)
            e[0][i] = fma(up, down, e[0][i1]);
            if (!isfinite(e[0][i])) {
                // Check for numerical instability (e.g., division by zero, overflow).
                max = k + i1; // Adjust max to avoid further unstable steps.
                break;
            }

        }

        std::swap(e[0], e[1]); // Swap rows for the next iteration.
    }

    if (!isfinite(e[max & static_cast<K>(1)][0])) {
        // Final check: only even rows (ε₂ₖ) are valid approximations.
        throw std::overflow_error("division by zero");
    }

    return e[max & static_cast<K>(1)][0]; // Return the transformed value.
}