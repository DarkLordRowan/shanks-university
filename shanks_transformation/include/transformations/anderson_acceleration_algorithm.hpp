#ifndef ANDERSON_ACCELERATION_ALGORITHM_HPP
#define ANDERSON_ACCELERATION_ALGORITHM_HPP
#pragma once

#define ANDERSON_DEFAULT_MAX_ORDER 50
#define ANDERSON_DEFAULT_BETA      1.0
#define ANDERSON_DEFAULT_SAFEGUARD 1.0e-12

/**
 * @file anderson_acceleration_algorithm.hpp
 * @brief Anderson Acceleration algorithm implementation.
*/

#include "series_acceleration.hpp"

#include <vector>
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

    using float_type = GetUnderlyingType<T>::value; //type in case of complex or interval

    explicit anderson_acceleration_algorithm(
        const K m = ANDERSON_DEFAULT_MAX_ORDER,
        const float_type& beta = utils::cast<float_type>(ANDERSON_DEFAULT_BETA),
        const float_type& safeguard = utils::cast<float_type>(ANDERSON_DEFAULT_SAFEGUARD)
    ) : series_acceleration<T, K>("anderson acceleration algorithm"),
        m_(m > 0 ? m : 1),
        safeguard_(safeguard)
    { update_beta(beta); }

    T operator() (
        const K n,
        const K order,
        const series_result<T>& data
    ) const override;

    void update_beta(const float_type& new_beta) {
        beta_ = (new_beta <= utils::cast<float_type>(0.0) || new_beta > utils::cast<float_type>(1.0) ? utils::cast<float_type>(1.0) : new_beta);
    }

private:

    K m_;                    ///< Memory depth for Anderson acceleration
    float_type beta_;        ///< Mixing parameter for damping
    float_type safeguard_;   ///< Small value to prevent division by zero

    inline T aitken_case( const K n, const std::vector<T>& Sn ) const;

    inline T main_case( const K n, const std::vector<T>& Sn ) const;

};


// ======================= OPERATOR IMPLEMENTATION ==========================

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::aitken_case(
    const K n,
    const std::vector<T>& Sn
) const {
    
    T denominator, accelerated;
    denominator = accelerated = utils::cast<T>(0.0);

    if constexpr (is_precisable<T>::value) utils::set_precision(utils::get_precision(Sn[0]), denominator, accelerated);

    using std::abs;
    using std::max;

    denominator = Sn[n] - utils::cast<T>(2.0) * Sn[n - 1] + Sn[n - 2];

    if (abs(denominator) < safeguard_ * max(abs(Sn[n]), max(abs(Sn[n - 1]), abs(Sn[n - 2])))) return Sn[n];  // fallback

    accelerated = Sn[n] - (Sn[n] - Sn[n - 1]) * (Sn[n] - Sn[n - 1]) / abs(denominator);
    return utils::cast<T>(beta_) * accelerated + utils::cast<T>(utils::cast<float_type>(1.0) - beta_) * Sn[n];
}

template <AcceptedLike T, UnsignedIntLike K>
inline T anderson_acceleration_algorithm<T, K>::main_case(
    const K n,
    const std::vector<T>& Sn
) const {

    using std::abs;
    using std::max;

    const K actual_m = std::min(m_, static_cast<K>(n - 1));
    
    std::vector<T> f(actual_m, utils::cast<T>(0.0));
    std::vector<T> b(actual_m, utils::cast<T>(0.0));
    std::vector<T> alpha(actual_m, utils::cast<T>(0.0));

    std::vector<std::vector<T>> A(actual_m, std::vector<T>(actual_m, utils::cast<T>(0.0)));
    std::vector<std::vector<T>> delta_S(actual_m, std::vector(actual_m, utils::cast<T>(0.0)));

    T sum, sum_b, factor, accelerated;
    sum = sum_b = factor = accelerated = utils::cast<T>(0.0);

    float_type max_val = utils::cast<float_type>(0.0);
    
    if constexpr (is_precisable<T>::value){

        utils::set_precision(utils::get_precision(Sn[0]), sum, sum_b, factor, accelerated, max_val);

        utils::set_vec_precision(f, utils::get_precision(Sn[0]));
        utils::set_vec_precision(b, utils::get_precision(Sn[0]));
        utils::set_vec_precision(alpha, utils::get_precision(Sn[0]));

        for(size_t j = 0; j < actual_m; ++j) utils::set_vec_precision(delta_S[j], utils::get_precision(Sn[0]));
        for(size_t j = 0; j < actual_m; ++j) utils::set_vec_precision(A[j], utils::get_precision(Sn[0]));

    }

    // --- Anderson m>1 ---
    
    K idx;
    for(K i = 0; i < actual_m; ++i){
        f[i] += Sn.at(n - actual_m + 1) - Sn.at(n - actual_m);
        for(K j = 0; j < actual_m; ++j){
            idx = n - actual_m + j;
            delta_S[i][j] += (idx < n ? Sn.at(idx + 1) - Sn.at(idx) : utils::cast<T>(0.0));
        }
    }
    // Normal equations A = ΔSᵀ ΔS
    
    for (K i = 0; i < actual_m; ++i) {
        for (K j = 0; j < actual_m; ++j) 
            for (K k = 0; k < actual_m; ++k) A[i][j] += delta_S[k][i] * delta_S[k][j];

        for (K k = 0; k < actual_m; ++k) b[i] += delta_S[k][i] * f[k];
    }

    // Add safeguard to diagonal
    for (K i = 0; i < actual_m; ++i) A[i][i] += utils::cast<T>(safeguard_);

    
    K pivot;
    // Gaussian elimination
    for (K i = 0; i < actual_m; ++i) {

        pivot = i;
        max_val = abs(A[i][i]) ;

        for (K j = i + 1; j < actual_m; ++j) 
            if (abs(A[j][i]) > max_val) {
                max_val = abs(A[j][i]);
                pivot = j;
            }
    
        if (max_val < safeguard_) {
            alpha.assign(actual_m, utils::cast<T>(1.0) / utils::cast<T>((actual_m)));
            break;
        }

        if (pivot != i) {
            std::swap(A[i], A[pivot]);
            std::swap(b[i], b[pivot]);
        }

        for (K j = i + 1; j < actual_m; ++j) {
            factor += A[j][i] / A[i][i] - factor;
            for (K k = i; k < actual_m; ++k) A[j][k] -= factor * A[i][k];
            b[j] -= factor * b[i];
        }
    }

    for (int i = static_cast<int>(actual_m) - 1; i >= 0; --i) {
        sum -= sum;
        for (K j = static_cast<K>(i) + 1; j < actual_m; ++j) sum += A[i][j] * alpha[j];
        alpha[i] += (b[i] - sum) / A[i][i] - alpha[i];
    }

    accelerated = Sn[n];
    for (K i = 0; i < actual_m; ++i) {
        idx = n - actual_m + i;
        accelerated -= alpha[i] * (Sn[idx + 1] - Sn[idx]);
    }

    return accelerated * utils::cast<T>(beta_) + Sn[n] * utils::cast<T>(utils::cast<float_type>(1.0) - beta_);
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
            utils::to_string(data.Sn.size()) +
            ", required at least " +
            utils::to_string(n + 1)
        );

    if (n < 2) return data.Sn[n];

    const K actual_m = std::min(m_, static_cast<K>(n - 1));

    const T result = (actual_m == 1 ? aitken_case(n, data.Sn) : main_case(n, data.Sn));

    if(!utils::isfinite(result)){
        throw std::overflow_error("division by zero");
    }
    
    return result;

}

#endif