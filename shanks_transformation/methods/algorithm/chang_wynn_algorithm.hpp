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
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
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
template <AcceptedLike T, UnsignedIntLike K>
class chang_wynn_algorithm final : public series_acceleration<T, K>
{
public:

    /**
     * @brief Parameterized constructor to initialize the chang_wynn_algorithm.
     * @param series The series class object to be accelerated.
     *        Must be a valid object implementing the required series interface.
     *        Purpose: To provide the series data for acceleration.
     */
    explicit chang_wynn_algorithm() : series_acceleration<T, K>("chang wynn") {}

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
	T operator()(
        const K n, 
        const K order,
		const series_result<T>& data
    ) const override;

};


template <AcceptedLike T, UnsignedIntLike K>
T chang_wynn_algorithm<T, K>::operator()(const K n, const K order, const series_result<T>& data) const {

    const K required_size = n + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for chann_wynn_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

	// For theory, see: Ford & Sidi (1987), Section 1 - Input validation
	// The algorithm requires at least one term for meaningful computation
	if (n == static_cast<K>(0))
		throw std::domain_error("n = 0 in the input");

	using std::isfinite;
    using std::fma;

    T up = static_cast<T>(0.0);
    T down = static_cast<T>(0.0);
    T coef = static_cast<T>(0.0);
    T coef2 = static_cast<T>(0.0);

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
            static_cast<T>(0.0)
        )
    ); // Two vectors of length n containing Epsilon table (current and previous rows).

    // For theory, see: Chang et al. (2019), Eq. (3.20d)
    // Vector F stores intermediate factors F₁⁽ⁿ⁾ used in the recursion.
    std::vector<T> f(
        n, 
        static_cast<T>(0.0)
    ); // Vector for containing F results from index 0 to n-1.

    // For theory, see: Wynn (1956), Eq. (2.6b)
    // Epsilon algorithm recursion: εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
    // Initialize the first row of the epsilon table.
    for (K i = static_cast<K>(0); i < max; ++i) {
        // For theory, see: Wynn (1956), Eq. (2.8)
        // ε₁⁽ⁿ⁾ = 1 / ΔSₙ for n >= 0.
        e[0][i] = static_cast<T>(1) / (data.an.at(i + static_cast<K>(1)));
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
            data.Sn.at(i2),
            data.Sn.at(i3) + data.Sn.at(i1)
        );

        // Compute Δ²S_n = S_{n+2} - 2S_{n+1} + S_n
        coef2 = fma(
            static_cast<T>(-2),
            data.Sn.at(i1),
            data.Sn.at(i2) + data.Sn.at(i)
        );

        // Numerator: ΔS_n * ΔS_{n+1} * Δ²S_{n+1}
        up = data.an.at(i1);
        up*= data.an.at(i2);
        up*= coef;

        // Denominator: ΔS_{n+2} * Δ²S_n - ΔS_n * Δ²S_{n+1}
        down = data.an.at(i3) * coef2;
        down -= data.an.at(i1) * coef;
        down = static_cast<T>(1) / down; // Reciprocal for division.

        // Compute T₂⁽ⁿ⁾ = S_{n+1} - (up * down)
        e[1][i] = static_cast<T>(fma(-up, down, data.Sn.at(i1)));

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

    if constexpr (isComplexLike<T>::value){
        if (!isfinite(e[max & static_cast<K>(1)][0].real()) || !isfinite(e[max & static_cast<K>(1)][0].imag())){
            throw std::overflow_error("division by zero");
        }
    } else {
        if(!isfinite(e[max & static_cast<K>(1)][0])){
            throw std::overflow_error("division by zero");
        }
    }

    return e[max & static_cast<K>(1)][0]; // Return the transformed value.
}

#ifdef INC_FPRECISION

    template <UnsignedIntLike K>
    class chang_wynn_algorithm<float_precision, K> final : public series_acceleration<float_precision, K>
    {
    public:
        explicit chang_wynn_algorithm() : series_acceleration<float_precision, K>("chang wynn") {}

    	float_precision operator()(
            const K n, 
            const K order,
    		const series_result<float_precision>& data
        ) const override;

    };

    template <UnsignedIntLike K>
    float_precision chang_wynn_algorithm<float_precision, K>::operator()(
        const K n, 
        const K order, 
        const series_result<float_precision>& data
    ) const {

        const K required_size = n + static_cast<K>(1);

        if (data.Sn.size() < required_size || data.an.size() < required_size){
            throw std::out_of_range("The Sn or an smaller then required for chang_wynn_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
    	}

    	// For theory, see: Ford & Sidi (1987), Section 1 - Input validation
    	// The algorithm requires at least one term for meaningful computation
    	if (n == static_cast<K>(0))
    		throw std::domain_error("n = 0 in the input");

    	using std::isfinite;
        using std::fma;

        const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

        float_precision up    = float_precision(0, precision);
        float_precision down  = float_precision(0, precision);
        float_precision coef  = float_precision(0, precision);
        float_precision coef2 = float_precision(0, precision);

        // For theory, see: Chang et al. (2019), Section 3.4, Eq. (3.20)
        // Initialization of epsilon table with modified initial conditions.
        K i1, i2, i3, k1;
        K max = n - (n & static_cast<K>(1)); // Ensure max is even for algorithm stability.

        // For theory, see: Wynn (1956), Section 3 (Algorithm)
        // Epsilon table structure: two rows to store intermediate transformations.
        std::vector<std::vector<float_precision>> e(
            2,
            std::vector<float_precision>(
                n,
                float_precision(0, precision)
            )
        ); // Two vectors of length n containing Epsilon table (current and previous rows).

        // For theory, see: Chang et al. (2019), Eq. (3.20d)
        // Vector F stores intermediate factors F₁⁽ⁿ⁾ used in the recursion.
        std::vector<float_precision> f(
            n, 
            float_precision(0, precision)
        ); // Vector for containing F results from index 0 to n-1.

        // For theory, see: Wynn (1956), Eq. (2.6b)
        // Epsilon algorithm recursion: εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
        // Initialize the first row of the epsilon table.
        for (K i = static_cast<K>(0); i < max; ++i) {
            // For theory, see: Wynn (1956), Eq. (2.8)
            // ε₁⁽ⁿ⁾ = 1 / ΔSₙ for n >= 0.
            e[0][i] = float_precision(1) / (data.an.at(i + static_cast<K>(1)));
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
                float_precision(-2),
                data.Sn.at(i2),
                data.Sn.at(i3) + data.Sn.at(i1)
            );

            // Compute Δ²S_n = S_{n+2} - 2S_{n+1} + S_n
            coef2 = fma(
                float_precision(-2),
                data.Sn.at(i1),
                data.Sn.at(i2) + data.Sn.at(i)
            );

            // Numerator: ΔS_n * ΔS_{n+1} * Δ²S_{n+1}
            up = data.an.at(i1);
            up*= data.an.at(i2);
            up*= coef;

            // Denominator: ΔS_{n+2} * Δ²S_n - ΔS_n * Δ²S_{n+1}
            down = data.an.at(i3) * coef2;
            down -= data.an.at(i1) * coef;
            down = float_precision(1) / down; // Reciprocal for division.

            // Compute T₂⁽ⁿ⁾ = S_{n+1} - (up * down)
            e[1][i] = fma(-up, down, data.Sn.at(i1));

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
                up = fma(float_precision(k), f[i], float_precision(k1));

                // Denominator: 1 / (Tₖ⁽ⁿ⁺¹⁾ - Tₖ⁽ⁿ⁾)
                down = float_precision(1);
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

        if(!isfinite(e[max & static_cast<K>(1)][0])){
            throw std::overflow_error("division by zero");
        }

        return e[max & static_cast<K>(1)][0]; // Return the transformed value.
    }

    #ifdef INC_COMPLEXPRECISION
        template <UnsignedIntLike K>
        class chang_wynn_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K>
        {
        public:
            explicit chang_wynn_algorithm() : series_acceleration<complex_precision<float_precision>, K>("chang wynn") {}
        
        	complex_precision<float_precision> operator()(
                const K n, 
                const K order,
        		const series_result<complex_precision<float_precision>>& data
            ) const override;
        
        };

        template <UnsignedIntLike K>
        complex_precision<float_precision> chang_wynn_algorithm<complex_precision<float_precision>, K>::operator()(
            const K n, 
            const K order, 
            const series_result<complex_precision<float_precision>>& data
        ) const {
        
            K required_size = n + static_cast<K>(1);
        
            if (data.Sn.size() < required_size || data.an.size() < required_size){
                throw std::out_of_range("The Sn or an smaller then required for chang_wynn_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
                "the size of Sn and an must be at least " + to_string(required_size));
        	}
        
        	// For theory, see: Ford & Sidi (1987), Section 1 - Input validation
        	// The algorithm requires at least one term for meaningful computation
        	if (n == static_cast<K>(0))
        		throw std::domain_error("n = 0 in the input");
        
            const size_t precision = std::max(
            std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()), 
            std::max(data.an[0].real().precision(),data.an[0].imag().precision())
            );

            complex_precision<float_precision> up    = complex_precision<float_precision>(
                float_precision(0, precision), 
                float_precision(0, precision)
            );
            complex_precision<float_precision> down  = complex_precision<float_precision>(
                float_precision(0, precision), 
                float_precision(0, precision)
            );
            complex_precision<float_precision> coef  = complex_precision<float_precision>(
                float_precision(0, precision), 
                float_precision(0, precision)
            );
            complex_precision<float_precision> coef2 = complex_precision<float_precision>(
                float_precision(0, precision), 
                float_precision(0, precision)
            );

            // For theory, see: Chang et al. (2019), Section 3.4, Eq. (3.20)
            // Initialization of epsilon table with modified initial conditions.
            K i1, i2, i3, k1;
            K max = n - (n & static_cast<K>(1)); // Ensure max is even for algorithm stability.

            // For theory, see: Wynn (1956), Section 3 (Algorithm)
            // Epsilon table structure: two rows to store intermediate transformations.
            std::vector<std::vector<complex_precision<float_precision>>> e(
                2,
                std::vector<complex_precision<float_precision>>(
                    n,
                    complex_precision<float_precision>(
                        float_precision(0, precision), 
                        float_precision(0, precision)
                    )
                )
            ); // Two vectors of length n containing Epsilon table (current and previous rows).

            // For theory, see: Chang et al. (2019), Eq. (3.20d)
            // Vector F stores intermediate factors F₁⁽ⁿ⁾ used in the recursion.
            std::vector<complex_precision<float_precision>> f(
                n, 
                complex_precision<float_precision>(
                    float_precision(0, precision), 
                    float_precision(0, precision)
                )
            ); // Vector for containing F results from index 0 to n-1.

            // For theory, see: Wynn (1956), Eq. (2.6b)
            // Epsilon algorithm recursion: εₖ₊₁⁽ⁿ⁾ = εₖ₋₁⁽ⁿ⁺¹⁾ + 1/(εₖ⁽ⁿ⁺¹⁾ - εₖ⁽ⁿ⁾)
            // Initialize the first row of the epsilon table.
            for (K i = static_cast<K>(0); i < max; ++i) {
                // For theory, see: Wynn (1956), Eq. (2.8)
                // ε₁⁽ⁿ⁾ = 1 / ΔSₙ for n >= 0.
                e[0][i] = complex_precision<float_precision>(1) / (data.an.at(i + static_cast<K>(1)));
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
                    complex_precision<float_precision>(-2),
                    data.Sn.at(i2),
                    data.Sn.at(i3) + data.Sn.at(i1)
                );

                // Compute Δ²S_n = S_{n+2} - 2S_{n+1} + S_n
                coef2 = fma(
                    complex_precision<float_precision>(-2),
                    data.Sn.at(i1),
                    data.Sn.at(i2) + data.Sn.at(i)
                );

                // Numerator: ΔS_n * ΔS_{n+1} * Δ²S_{n+1}
                up = data.an.at(i1);
                up*= data.an.at(i2);
                up*= coef;

                // Denominator: ΔS_{n+2} * Δ²S_n - ΔS_n * Δ²S_{n+1}
                down = data.an.at(i3) * coef2;
                down -= data.an.at(i1) * coef;
                down = complex_precision<float_precision>(1) / down; // Reciprocal for division.

                // Compute T₂⁽ⁿ⁾ = S_{n+1} - (up * down)
                e[1][i] = fma(-up, down, data.Sn.at(i1));

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
                    up = fma(complex_precision<float_precision>(k), f[i], complex_precision<float_precision>(k1));

                    // Denominator: 1 / (Tₖ⁽ⁿ⁺¹⁾ - Tₖ⁽ⁿ⁾)
                    down = complex_precision<float_precision>(1) / (e[1][i1] - e[1][i]);

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

            if (!isfinite(e[max & static_cast<K>(1)][0].real()) || !isfinite(e[max & static_cast<K>(1)][0].imag())){
                throw std::overflow_error("division by zero");
            }

            return e[max & static_cast<K>(1)][0]; // Return the transformed value.
        }
    #endif
#endif