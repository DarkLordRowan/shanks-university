#ifndef HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#define HALF_MINUS_SINX_MULTI_PI_4_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file half_minus_sinx_multi_pi_4_iterator.hpp
 * @brief Iterator for the series expansion of 0.5 - (pi/4)*sin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = 0.5 - (pi/4) * sin(x).
 *
 * This class implements a specific series expansion for the given trigonometric function,
 * typically valid for x within the range [0, pi/2].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class half_minus_sinx_multi_pi_4_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for half_minus_sinx_multi_pi_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit half_minus_sinx_multi_pi_4_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (0.5 - (pi/4)*sin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.5 - (pi/4)*sin(x).
     */
    T get_sum() const override {
        return utils::cast<T>::meta(0.5) - utils::cast<T>::meta(std::numbers::pi * 0.25) * utils::math<T>::sin(this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is outside [0, pi/2] or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        if constexpr (isComplexLike<T>::value) {
            return !utils::helpers<T>::isfinite(this->x) || this->x.real() < utils::cast<float_type>(0) ||
                   this->x.real() > utils::cast<float_type>(0.5 * std::numbers::pi);
        } else {
            return !utils::helpers<T>::isfinite(this->x) || this->x < utils::cast<T>::meta(0) ||
                   this->x > utils::cast<T>::meta(0.5 * std::numbers::pi);
        }
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Specific term formula for the expansion of 0.5 - (pi/4)*sin(x)
        state = utils::math<T>::cos(utils::cast<T>::meta(utils::math<size_t>::fma(
                                        static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(2))) *
                                    this->x) /
                utils::cast<T>::meta(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(3)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
