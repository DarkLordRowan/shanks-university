/**
 * @file anderson_acceleration_algorithm.hpp
 * @brief Anderson Acceleration algorithm implementation.
 */

#pragma once

#include "series_acceleration.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>

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

    using Kostil = std::conditional_t<isFloatLike<T>::value, TypeWrapper<T>, T>;
    using param_type = typename Kostil::value_type; // TYPE FOR FLOATLIKE IN THE SITUATION OF COMPLEX OR INTERVAL

    explicit anderson_acceleration_algorithm(
        K m = 5,
        param_type beta = static_cast<param_type>(1.0),
        param_type safeguard = static_cast<param_type>(1e-12)
    ) : series_acceleration<T, K>("anderson acceleration algorithm"),
        m_(m > 0 ? m : 1),
        safeguard_(safeguard)
    { update_beta(beta); }

    T operator() (
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;

    void update_beta(const param_type& new_beta) {
        beta_ = (new_beta <= static_cast<param_type>(0) || new_beta > static_cast<param_type>(1) ? static_cast<param_type>(1) : new_beta);
    }

private:

    K m_;                    ///< Memory depth for Anderson acceleration
    param_type beta_;        ///< Mixing parameter for damping
    param_type safeguard_;   ///< Small value to prevent division by zero

    inline T aitken_case( const K n, const std::vector<T>& Sn ) const;

    inline T main_case( const K n, const std::vector<T>& Sn ) const;

};


// ======================= OPERATOR IMPLEMENTATION ==========================

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::aitken_case(
    const K n,
    const std::vector<T>& Sn
) const {
    
    T denominator, delta, accelerated;
    denominator = delta = accelerated = static_cast<T>(0);

    param_type abs_denom, max_val;
    abs_denom = max_val = static_cast<param_type>(0);

    #ifdef INC_FPRECISION
    if constexpr (is_precisable<T>::value){
        const size_t precision = utils::get_precision(Sn[0]);
        utils::set_precision(precision, denominator, delta, accelerated, abs_denom);
    }
    #endif

    using std::abs;
    using std::max;

    denominator = Sn[n] - static_cast<T>(2) * Sn[n - 1] + Sn[n - 2];

    abs_denom= abs(denominator);

    max_val = max(abs(Sn[n]), max(abs(Sn[n - 1]), abs(Sn[n - 2])));

    if (abs_denom < safeguard_ * max_val) return Sn[n];  // fallback

    delta = Sn[n] - Sn[n - 1];
    accelerated = Sn[n] - delta * delta / denominator;
    return static_cast<T>(beta_) * accelerated + static_cast<T>(static_cast<param_type>(1) - beta_) * Sn[n];
}

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::main_case(
    const K n,
    const std::vector<T>& Sn
) const {

    using std::abs;
    using std::max;

    const K actual_m = std::min(m_, n - 1);
    
    std::vector<T> f(actual_m, static_cast<T>(0));
    std::vector<T> b(actual_m, static_cast<T>(0));
    std::vector<T> alpha(actual_m, static_cast<T>(0));

    std::vector<std::vector<T>> A(actual_m, std::vector<T>(actual_m, static_cast<T>(0)));
    std::vector<std::vector<T>> delta_S(actual_m, std::vector(actual_m, static_cast<T>(0)));

    T sum, sum_b, factor, accelerated;
    sum = sum_b = factor = accelerated = static_cast<T>(0);

    param_type max_val = static_cast<param_type>(0);
    
    #ifdef INC_FPRECISION
    if constexpr (is_precisable<T>::value){

        const size_t precision = utils::get_precision(Sn[0]);

        utils::set_precision(precision, sum, sum_b, factor, accelerated, max_val);

        utils::set_vec_precision(f, precision);
        utils::set_vec_precision(b, precision);
        utils::set_vec_precision(alpha, precision);

        for(size_t j = 0; j < actual_m; ++j) utils::set_vec_precision(delta_S[j], precision);
        for(size_t j = 0; j < actual_m; ++j) utils::set_vec_precision(A[j], precision);

    }
    #endif

    // --- Anderson m>1 ---
    
    K idx;
    for(K i = 0; i < actual_m; ++i){
        f[i] += Sn.at(n - actual_m + 1) - Sn.at(n - actual_m);
        for(K j = 0; j < actual_m; ++j){
            idx = n - actual_m + j;
            delta_S[i][j] += (idx < n ? Sn.at(idx + 1) - Sn.at(idx) : static_cast<T>(0));
        }
    }
    // Normal equations A = ΔSᵀ ΔS
    
    for (K i = 0; i < actual_m; ++i) {

        for (K j = 0; j < actual_m; ++j) {
            sum = static_cast<T>(0);
            for (K k = 0; k < actual_m; ++k) sum += delta_S[k][i] * delta_S[k][j];
            A[i][j] = sum;
        }

        sum_b = static_cast<T>(0);
        for (K k = 0; k < actual_m; ++k) sum_b += delta_S[k][i] * f[k];

        b[i] = sum_b;
    }

    // Add safeguard to diagonal
    for (K i = 0; i < actual_m; ++i) A[i][i] += static_cast<T>(safeguard_);

    
    K pivot;
    // Gaussian elimination
    for (K i = 0; i < actual_m; ++i) {

        pivot = i;
        max_val = abs(A[i][i]);

        for (K j = i + 1; j < actual_m; ++j) 
            if (abs(A[j][i]) > max_val) {
                max_val = abs(A[j][i]);
                pivot = j;
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
            factor = A[j][i] / A[i][i];
            for (K k = i; k < actual_m; ++k) A[j][k] -= factor * A[i][k];
            b[j] -= factor * b[i];
        }
    }

    for (int i = static_cast<int>(actual_m) - 1; i >= 0; --i) {
        sum = static_cast<T>(0);
        for (K j = static_cast<K>(i) + 1; j < actual_m; ++j) sum += A[i][j] * alpha[j];
        alpha[i] = (b[i] - sum) / A[i][i];
    }

    accelerated = Sn[n];
    for (K i = 0; i < actual_m; ++i) {
        idx = n - actual_m + i;
        accelerated -= alpha[i] * (Sn[idx + 1] - Sn[idx]);
    }

    return accelerated * static_cast<T>(beta_) + Sn[n] * static_cast<T>(static_cast<param_type>(1) - beta_);
}

template <AcceptedLike T, UnsignedIntLike K>
T anderson_acceleration_algorithm<T, K>::operator()(
    const K n,
    const K /*order*/,
    const series_result<T>& data
) const {


    if (data.Sn.size() < n + 1)
        throw std::out_of_range(
            "Insufficient data in Sn vector: size=" +
            std::to_string(data.Sn.size()) +
            ", required at least " +
            std::to_string(n + 1)
        );

    if (n < 2) return data.Sn[n];

    const K actual_m = std::min(m_, n - 1);

    const T result = (actual_m == 1 ? aitken_case(n, data.Sn) : main_case(n, data.Sn));

    if constexpr (isComplexLike<T>::value){
        if (!isfinite(result.real()) || !isfinite(result.imag())){
            throw std::overflow_error("division by zero");
        }
    } else {
        if(!isfinite(result)){
            throw std::overflow_error("division by zero");
        }
    }
    
    return result;

}