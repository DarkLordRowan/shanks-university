#ifndef ANDERSON_ACCELERATION_ALGORITHM_HPP
#define ANDERSON_ACCELERATION_ALGORITHM_HPP
#pragma once

#define ANDERSON_DEFAULT_MAX_ORDER 50
#define ANDERSON_DEFAULT_BETA 1.0
#define ANDERSON_DEFAULT_SAFEGUARD 1.0e-12

/**
 * @file anderson_acceleration_algorithm.hpp
 * @brief Anderson Acceleration algorithm implementation.
 * @authors Mkhitaryan A.A., Gerasimov A.I.
 */

namespace shanks {
namespace algos {

/**
 * @brief Anderson acceleration algorithm class template implementing sequence transformation.
 *
 * This class provides a robust implementation of the Anderson acceleration (also known as Pulay mixing),
 * which is used to speed up the convergence of fixed-point iterations. It is particularly effective
 * for slowly converging sequences by utilizing information from previous iterations to find an
 * optimal linear combination of recent steps.
 *
 * @authors Mkhitaryan A.A., Gerasimov A.I.
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all mathematical computations and storage of series terms
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 */
template <AcceptedLike T, UnsignedIntLike K>
class anderson_acceleration_algorithm final : public series_acceleration<T, K> {
public:
    using float_type = real_of<T>::value;  // type in case of complex or interval

    /**
     * @brief Parameterized constructor to initialize the Anderson Acceleration Algorithm.
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param m The maximum depth of history (memory) to consider for acceleration.
     * @param beta The mixing parameter (damping factor) between 0 and 1.
     * @param safeguard A small value used for numerical stability to prevent division by zero.
     */
    explicit anderson_acceleration_algorithm(
        const K m = ANDERSON_DEFAULT_MAX_ORDER,
        const float_type& beta = utils::cast<float_type, double>()(ANDERSON_DEFAULT_BETA),
        const float_type& safeguard = utils::cast<float_type, double>()(ANDERSON_DEFAULT_SAFEGUARD))
        : series_acceleration<T, K>("anderson acceleration algorithm"), m_(m > 0 ? m : 1), safeguard_(safeguard) {
        update_beta(beta);
    }

    /**
     * @brief Executes the Anderson acceleration on the provided partial sums.
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n The current index in the sequence to accelerate.
     * @param order The transformation order (unused in this specific algorithm, kept for interface consistency).
     * @param data A series_result structure containing the partial sums (Sn).
     * @return T The accelerated value of the series at index n.
     * @throws std::out_of_range if the provided Sn vector is too small.
     * @throws std::overflow_error if a division by zero or non-finite result occurs.
     */
    T operator()(const K n, const K order, const series_result<T>& data) const override;

    /**
     * @brief Setter to update the beta parameter (damping factor).
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param new_beta The new beta value. If it's outside the (0, 1] range, it defaults to 1.0.
     */
    void update_beta(const float_type& new_beta) {
        beta_ = (new_beta <= utils::cast<float_type, int>()(0) || new_beta > utils::cast<float_type, int>()(1)
                     ? utils::cast<float_type, int>()(1)
                     : new_beta);
    }

private:
    K m_;                  /**< Memory depth for Anderson acceleration  */
    float_type beta_;      /**< Mixing parameter for damping            */
    float_type safeguard_; /**< Small value to prevent division by zero */

    /**
     * @brief Handles the special case where memory depth is effectively 1 (Aitken-like acceleration).
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n Index of the term to process.
     * @param Sn Reference to the vector of partial sums.
     * @return T The accelerated value.
     */
    inline T aitken_case(const K n, const std::vector<T>& Sn) const;

    /**
     * @brief Main logic for Anderson acceleration when memory depth m > 1.
     *
     * @authors Mkhitaryan A.A., Gerasimov A.I.
     * @param n Index of the term to process.
     * @param Sn Reference to the vector of partial sums.
     * @return T The accelerated value calculated using the history of previous terms.
     */
    inline T main_case(const K n, const std::vector<T>& Sn) const;

    static inline std::size_t how_much(const std::size_t n) { return n + 1; }
};

// ======================= OPERATOR IMPLEMENTATION ==========================

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::aitken_case(const K n, const std::vector<T>& Sn) const {
    const size_t precision = utils::helpers<T>::get_precision(Sn[0]);

    T denominator, accelerated;
    denominator = accelerated = utils::cast<T, int>()(0, precision);

    // Calculate the second-order difference for the denominator
    denominator = Sn[n] - utils::cast<T, int>()(2, precision) * Sn[n - 1] + Sn[n - 2];

    // Check for numerical stability. If the denominator is too small, fallback to the current term.
    if (utils::math<T>::abs(denominator) <
        safeguard_ * std::max(utils::math<T>::abs(Sn[n]),
                              std::max(utils::math<T>::abs(Sn[n - 1]), utils::math<T>::abs(Sn[n - 2]))))
        return Sn[n];  // fallback

    // Standard Aitken formula followed by mixing with the original term
    accelerated = Sn[n] - (Sn[n] - Sn[n - 1]) * (Sn[n] - Sn[n - 1]) /
                              utils::cast<T, float_type>()(utils::math<T>::abs(denominator));
    return utils::cast<T, float_type>()(beta_, precision) * accelerated +
           utils::cast<T, float_type>()(utils::cast<float_type, int>()(1, precision) - beta_, precision) * Sn[n];
}

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::main_case(const K n, const std::vector<T>& Sn) const {
    // Determine the actual history size to use, capped by m_ and current index n
    const K actual_m = std::min(m_, static_cast<K>(n - 1));
    const size_t precision = utils::helpers<T>::get_precision(Sn[0]);

    std::vector<T> f(actual_m, utils::cast<T, int>()(0, precision));
    std::vector<T> b(actual_m, utils::cast<T, int>()(0, precision));
    std::vector<T> alpha(actual_m, utils::cast<T, int>()(0, precision));

    // Matrices for the least squares problem (normal equations)
    std::vector<std::vector<T>> A(actual_m, std::vector<T>(actual_m, utils::cast<T, int>()(0, precision)));
    std::vector<std::vector<T>> delta_S(actual_m, std::vector(actual_m, utils::cast<T, int>()(0, precision)));

    T sum, sum_b, factor, accelerated;
    sum = sum_b = factor = accelerated = utils::cast<T, int>()(0, precision);

    float_type max_val = utils::cast<float_type, int>()(0, precision);

    // --- Anderson m>1 logic ---

    K idx;
    // Construct the residuals and differences
    for (K i = 0; i < actual_m; ++i) {
        f[i] += Sn.at(n - actual_m + i + 1) - Sn.at(n - actual_m + i);
        for (K j = 0; j < actual_m; ++j) {
            idx = n - actual_m + j;
            delta_S[i][j] += (idx < n ? Sn.at(idx + 1) - Sn.at(idx) : utils::cast<T, int>()(0, precision));
        }
    }

    // Setup the Normal equations: A = delta_S.transpose() * delta_S
    for (K i = 0; i < actual_m; ++i) {
        for (K j = 0; j < actual_m; ++j)
            for (K k = 0; k < actual_m; ++k) A[i][j] += delta_S[k][i] * delta_S[k][j];

        for (K k = 0; k < actual_m; ++k) b[i] += delta_S[k][i] * f[k];
    }

    // Numerical regularization: add safeguard to the diagonal to avoid singularity
    for (K i = 0; i < actual_m; ++i) A[i][i] += utils::cast<T, float_type>()(safeguard_, precision);

    K pivot;
    // Solve the system A * alpha = b using Gaussian elimination with partial pivoting
    for (K i = 0; i < actual_m; ++i) {
        pivot = i;
        max_val = utils::math<T>::abs(A[i][i]);

        for (K j = i + 1; j < actual_m; ++j)
            if (utils::math<T>::abs(A[j][i]) > max_val) {
                max_val = utils::math<T>::abs(A[j][i]);
                pivot = j;
            }

        // If the matrix is singular despite regularization, use a simple average fallback
        if (max_val < safeguard_) {
            alpha.assign(actual_m, utils::cast<T, int>()(1, precision) / utils::cast<T, K>()(actual_m, precision));
            break;
        }

        // Swap rows for pivoting
        if (pivot != i) {
            std::swap(A[i], A[pivot]);
            std::swap(b[i], b[pivot]);
        }

        // Elimination step
        for (K j = i + 1; j < actual_m; ++j) {
            factor = A[j][i] / A[i][i];
            for (K k = i; k < actual_m; ++k) A[j][k] -= factor * A[i][k];
            b[j] -= factor * b[i];
        }
    }

    // Back substitution to find alpha coefficients
    for (int i = static_cast<int>(actual_m) - 1; i >= 0; --i) {
        sum = utils::cast<T, int>()(0, precision);
        for (K j = static_cast<K>(i) + 1; j < actual_m; ++j) sum += A[i][j] * alpha[j];
        alpha[i] = (b[i] - sum) / A[i][i];
    }

    // Calculate the final accelerated value as a combination of previous terms
    accelerated = Sn[n];
    for (K i = 0; i < actual_m; ++i) {
        idx = n - actual_m + i;
        accelerated -= alpha[i] * (Sn[idx + 1] - Sn[idx]);
    }

    // Final damping step
    return accelerated * utils::cast<T, float_type>()(beta_, precision) +
           Sn[n] * utils::cast<T, float_type>()(utils::cast<float_type, int>()(1, precision) - beta_, precision);
}

template <AcceptedLike T, UnsignedIntLike K>
T anderson_acceleration_algorithm<T, K>::operator()(const K n, const K /*order*/, const series_result<T>& data) const {
    // Validate that we have enough data points to perform acceleration
    if (data.Sn.size() < anderson_acceleration_algorithm<T, K>::how_much(n))
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" + utils::helpers<size_t>::to_string(data.Sn.size()) +
            ", required at least " + utils::helpers<K>::to_string(anderson_acceleration_algorithm<T, K>::how_much(n)));

    // Not enough points for acceleration, return original partial sum
    if (n < 2) return data.Sn[n];

    const K actual_m = std::min(m_, static_cast<K>(n - 1));

    // Choose case based on available history
    const T result = (actual_m == 1 ? aitken_case(n, data.Sn) : main_case(n, data.Sn));

    if (!utils::helpers<T>::isfinite(result)) throw std::overflow_error("division by zero");

    return result;
}

}  // namespace algos
}  // namespace shanks

#endif
