#ifndef EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP
#define EIGHTH_PI_M_ONE_THIRD_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file eighth_pi_m_one_third_iterator.hpp
 * @brief Iterator for the series expansion related to pi/8 - 1/3.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * (pi/8 - 1/3).
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * (pi/8 - 1/3).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class eighth_pi_m_one_third_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for eighth_pi_m_one_third_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit eighth_pi_m_one_third_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * (pi/8 - 1/3)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (pi/8 - 1/3).
     */
    T get_sum() const override {
        return this->x * (utils::cast<T, double>()(std::numbers::pi * 0.125) -
                          utils::cast<T, int>()(1) / utils::cast<T, int>()(3));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term of the specific series expansion
        state = utils::math<T>::template minus_one_raised_to_power_n<K>(n) * this->x /
                utils::cast<T, size_t>()(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(3)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(5)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
