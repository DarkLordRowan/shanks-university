#ifndef THREE_MINUS_PI_ITERATOR_HPP
#define THREE_MINUS_PI_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file three_minus_pi_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (3 - pi).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * (3 - pi).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (3 - pi).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class three_minus_pi_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for three_minus_pi_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit three_minus_pi_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (3 - pi).
     */
    T get_sum() const override { return this->x * (utils::cast<T>::meta(3) - utils::cast<T>::meta(std::numbers::pi)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T>::meta(0); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Alternating term formula: (-1)^(n+1) * x / ((n+1)*(n+2)*(2n+3))
        state = utils::minus_one_raised_to_power_n<T, K>(n + 1) * this->x /
                utils::cast<T>::meta(
                    (n + 1) * (n + 2) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(3)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
