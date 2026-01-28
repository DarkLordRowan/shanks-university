#ifndef PI_SQUARED_6_MINUS_ONE_ITERATOR_HPP
#define PI_SQUARED_6_MINUS_ONE_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_squared_6_minus_one_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (pi^2 / 6 - 1).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * (pi^2 / 6 - 1).
 *
 * This class implements a specific series expansion related to the Basel problem
 * (zeta(2)) whose sum is x * (pi^2 / 6 - 1).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_squared_6_minus_one_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_squared_6_minus_one_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_squared_6_minus_one_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (pi^2 / 6 - 1).
     */
    T get_sum() const override {
        return this->x * (utils::cast<T>::meta(std::numbers::pi) * utils::cast<T>::meta(std::numbers::pi) / utils::cast<T>::meta(6) -
                          utils::cast<T>::meta(1));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T>::meta(0); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (x / ((n+1)^2 * (n+2))).
     */
    T next(K n, T& state) const override {
        // Term formula derived from the expansion of (zeta(2) - 1) * x
        state = this->x / utils::cast<T>::meta((n + 1) * (n + 1) * (n + 2));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
