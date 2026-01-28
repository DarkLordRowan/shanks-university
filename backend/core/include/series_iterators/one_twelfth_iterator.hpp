#ifndef ONE_TWELFTH_ITERATOR_HPP
#define ONE_TWELFTH_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file one_twelfth_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x/12.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled linear function f(x) = x / 12.
 *
 * This class implements a specific series expansion whose analytic sum is x / 12.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class one_twelfth_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_twelfth_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit one_twelfth_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x / 12).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x / 12.
     */
    T get_sum() const override { return this->x / utils::cast<T>::meta(12); }

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
        // Formula for the n-th term of the expansion: x / ((2n+1)(2n+3)(2n+5))
        state = this->x /
                utils::cast<T>::meta(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(3)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(5)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
