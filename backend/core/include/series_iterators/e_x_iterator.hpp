#ifndef E_X_ITERATOR_HPP
#define E_X_ITERATOR_HPP

#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file e_x_iterator.hpp
 * @brief Iterator for the series expansion of the E_x(x) function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the complete elliptic integral related function E_x(x).
 *
 * This class implements a specific series expansion for E_x(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class e_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for e_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit e_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (E_x(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of E_x(x).
     */
    T get_sum() const override { return utils::e_x(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) >= utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the E_x(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term is pi/2, subsequent terms use the specialized recurrence for E_x
        if (n == 0)
            state = utils::cast<T>::meta(std::numbers::pi * 0.5, utils::helpers<T>::get_precision(state));
        else
            state *=
                this->x * this->x * (utils::cast<T>::meta((n - 1) * (n - 1)) - utils::cast<T>::meta(0.25)) / utils::cast<T>::meta(n * n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
