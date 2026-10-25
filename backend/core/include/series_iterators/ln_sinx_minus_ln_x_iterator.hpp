#ifndef LN_SINX_MINUS_LN_X_ITERATOR_HPP
#define LN_SINX_MINUS_LN_X_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file ln_sinx_minus_ln_x_iterator.hpp
 * @brief Iterator for the series expansion of ln(sin(x)) - ln(x).
 * @authors Bolshakov M.P.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = ln(sin(x)) - ln(x).
 *
 * This class implements an expansion for ln(sin(x)/x), which resolves the
 * singularity at x=0. It is typically valid for x within the range [0, pi).
 *
 * @authors Bolshakov M.P.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln_sinx_minus_ln_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln_sinx_minus_ln_x_iterator.
     * @authors Bolshakov M.P.
     */
    explicit ln_sinx_minus_ln_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (ln(sin(x)) - ln(x)).
     * @authors Bolshakov M.P.
     * @return T The value of ln(sin(x)/x).
     */
    T get_sum() const override {
        if(this->x==utils::cast<T,int>()(0)) return utils::cast<T,int>()(0);
        return utils::math<T>::log(utils::math<T>::sin(this->x)/this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Bolshakov M.P.
     * @return true if x is outside [0, pi) or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        const auto precision=utils::helpers<T>::get_precision(this->x);
        using R=typename real_of<T>::value;
        const R pi=utils::cast<R,int>()(4,precision)*utils::math<R>::atan(utils::cast<R,int>()(1,precision));
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x)>=pi;
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Bolshakov M.P.
     * @return T The next term of the series.
     */
    T next(K n,T& state) const override {
        // Euler sine product, including its removable x=0 limit: DLMF 4.22.
        const auto precision=utils::helpers<T>::get_precision(this->x);
        const T one=utils::cast<T,int>()(1,precision);
        const T pi=utils::cast<T,int>()(4,precision)*utils::math<T>::atan(one);
        const T k=utils::cast<T,K>()(n,precision)+one;
        const T ratio=this->x/(k*pi);
        state=utils::math<T>::log(one-ratio*ratio);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
