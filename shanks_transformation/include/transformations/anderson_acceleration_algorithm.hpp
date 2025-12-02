/**
 * @file anderson_acceleration_algorithm.hpp
 * @brief Anderson Acceleration algorithm implementation.
 */

#pragma once

#include "series_acceleration.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <numeric>

/**
 * @brief Anderson acceleration algorithm class template implementing sequence transformation.
 *
 * @authors
 *   Mkhitaryan A.A., Gerasimov A.I.
 */
template <AcceptedLike T, UnsignedIntLike K>
class anderson_acceleration_algorithm final : public series_acceleration<T, K>
{
public:
    explicit anderson_acceleration_algorithm(
        K m = 5,
        T beta = static_cast<T>(1.0),
        T safeguard = static_cast<T>(1e-12)
    ) : series_acceleration<T, K>("anderson acceleration algorithm"),
        m_(m > 0 ? m : 1),
        beta_(beta),
        safeguard_(safeguard)
    {
        if (beta_ <= static_cast<T>(0) || beta_ > static_cast<T>(1)) {
            beta_ = static_cast<T>(1);
        }
    }

    T operator()(
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;

private:
    K m_;           ///< Memory depth for Anderson acceleration
    T beta_;        ///< Mixing parameter for damping
    T safeguard_;   ///< Small value to prevent division by zero

    // Helper function for absolute value
    template<typename U>
    static U safe_abs(const U& x) {
        if constexpr (std::is_arithmetic_v<U> || is_precisable<U>::value) {
            return x < U(0) ? -x : x;
        } else if constexpr (isComplexLike<U>::value) {
            return x.abs();
        }
        return x; // fallback
    }

    // Safe precision setter (works only for float_precision)
    template<typename U>
    static void set_precision_if_needed(U& value, size_t precision) {
        if constexpr (std::is_same_v<U, float_precision>) {
            value.set_precision(precision);
        }
    }
};


// ======================= OPERATOR IMPLEMENTATION ==========================

template <AcceptedLike T, UnsignedIntLike K>
T anderson_acceleration_algorithm<T, K>::operator()(
    const K n,
    const K /*order*/,
    const series_result<T>& data
) const {

    if (data.Sn.size() < n + 1) {
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" +
            std::to_string(data.Sn.size()) +
            ", required at least " +
            std::to_string(n + 1)
        );
    }

    if (n < 2) {
        return data.Sn[n];
    }

    K actual_m = std::min(m_, n - 1);

    // --- Anderson m=1 (Aitken-like) ---
    if (actual_m == 1) {
        T S0 = data.Sn[n - 2];
        T S1 = data.Sn[n - 1];
        T S2 = data.Sn[n];

        T denominator = S2 - static_cast<T>(2) * S1 + S0;
        T abs_denom = safe_abs(denominator);

        T max_val = std::max(safe_abs(S2),
                        std::max(safe_abs(S1), safe_abs(S0)));

        if (abs_denom < safeguard_ * max_val) {
            return data.Sn[n];  // fallback
        }

        T delta = S2 - S1;
        T accelerated = S2 - delta * delta / denominator;

        return beta_ * accelerated + (static_cast<T>(1) - beta_) * data.Sn[n];
    }

    // --- Anderson m>1 ---
    std::vector<std::vector<T>> delta_S;
    std::vector<T> f;

    for (K i = n - actual_m; i < n; ++i) {
        f.push_back(data.Sn[i + 1] - data.Sn[i]);

        std::vector<T> row;
        for (K j = 0; j < actual_m; ++j) {
            K idx = n - actual_m + j;
            if (idx < n) {
                row.push_back(data.Sn[idx + 1] - data.Sn[idx]);
            } else {
                row.push_back(static_cast<T>(0));
            }
        }
        delta_S.push_back(row);
    }

    // Normal equations A = ΔSᵀ ΔS
    std::vector<std::vector<T>> A(actual_m, std::vector<T>(actual_m, static_cast<T>(0)));
    std::vector<T> b(actual_m, static_cast<T>(0));

    for (K i = 0; i < actual_m; ++i) {
        for (K j = 0; j < actual_m; ++j) {
            T sum = static_cast<T>(0);
            for (K k = 0; k < actual_m; ++k) {
                sum += delta_S[k][i] * delta_S[k][j];
            }
            A[i][j] = sum;
        }

        T sum_b = static_cast<T>(0);
        for (K k = 0; k < actual_m; ++k) {
            sum_b += delta_S[k][i] * f[k];
        }
        b[i] = sum_b;
    }

    // Add safeguard to diagonal
    for (K i = 0; i < actual_m; ++i) {
        A[i][i] += safeguard_;
    }

    std::vector<T> alpha(actual_m, static_cast<T>(0));

    // Gaussian elimination
    for (K i = 0; i < actual_m; ++i) {

        K pivot = i;
        T max_val = safe_abs(A[i][i]);

        for (K j = i + 1; j < actual_m; ++j) {
            if (safe_abs(A[j][i]) > max_val) {
                max_val = safe_abs(A[j][i]);
                pivot = j;
            }
        }

        if (max_val < safeguard_) {
            alpha.assign(actual_m, static_cast<T>(1) / static_cast<T>(actual_m));
            break;
        }

        if (pivot != i) {
            std::swap(A[i], A[pivot]);
            std::swap(b[i], b[pivot]);
        }

        for (K j = i + 1; j < actual_m; ++j) {
            T factor = A[j][i] / A[i][i];
            for (K k = i; k < actual_m; ++k) {
                A[j][k] -= factor * A[i][k];
            }
            b[j] -= factor * b[i];
        }
    }

    for (int i = static_cast<int>(actual_m) - 1; i >= 0; --i) {
        T sum = static_cast<T>(0);
        for (K j = static_cast<K>(i) + 1; j < actual_m; ++j) {
            sum += A[i][j] * alpha[j];
        }
        alpha[i] = (b[i] - sum) / A[i][i];
    }

    T accelerated = data.Sn[n];
    for (K i = 0; i < actual_m; ++i) {
        K idx = n - actual_m + i;
        accelerated -= alpha[i] * (data.Sn[idx + 1] - data.Sn[idx]);
    }

    return beta_ * accelerated + (static_cast<T>(1) - beta_) * data.Sn[n];
}

