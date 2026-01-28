#ifndef PI_SIX_MIN_HALF_ITERATOR_HPP
#define PI_SIX_MIN_HALF_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_six_min_half_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (pi/6 - 1/2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * (pi/6 - 1/2).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (pi/6 - 1/2).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_six_min_half_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_six_min_half_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_six_min_half_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (pi/6 - 1/2).
     */
    T get_sum() const override {
        return this->x * (utils::cast<T>(std::numbers::pi) / utils::cast<T>(6) - utils::cast<T>(0.5));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::isfinite(this->x) || this->x == utils::cast<T>(0); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Alternating term formula: (-1)^n * x / ((6n+5)(6n+7))
        state = utils::minus_one_raised_to_power_n<T, K>(n) * this->x /
                utils::cast<T>(utils::fma(static_cast<size_t>(6), static_cast<size_t>(n), static_cast<size_t>(5)) *
                               utils::fma(static_cast<size_t>(6), static_cast<size_t>(n), static_cast<size_t>(7)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
