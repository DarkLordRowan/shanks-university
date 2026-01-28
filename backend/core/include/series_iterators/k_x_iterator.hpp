#ifndef K_X_ITERATOR_HPP
#define K_X_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file k_x_iterator.hpp
 * @brief Iterator for the series expansion of the complete elliptic integral K(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the complete elliptic integral of the first kind K(x).
 *
 * This class implements the standard power series expansion for K(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class k_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for k_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit k_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (K(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the complete elliptic integral K(x).
     */
    T get_sum() const override { return utils::k_x(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) > utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the K(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term is pi/2, subsequent terms use the squared ratio of odd factorials
        if (n == 0)
            state = utils::cast<T>::meta(std::numbers::pi * 0.5, utils::helpers<T>::get_precision(state));
        else
            state *= this->x * this->x *
                     utils::cast<T>::meta(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                             static_cast<size_t>(1)) *
                                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                             static_cast<size_t>(1))) /
                     utils::cast<T>::meta(4 * n * n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
