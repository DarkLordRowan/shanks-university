#ifndef NATURAL_SERIES__ITERATOR_HPP
#define NATURAL_SERIES__ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file one_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the identity function f(x) = x.
 *
 * This class implements a specific series expansion whose analytic sum is
 * simply the value x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class natural_series_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit natural_series_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x.
     */
    T get_sum() const override { return utils::cast<T, int>()(-1, utils::helpers<T>::get_precision(this->x)) / utils::cast<T, int>()(12, utils::helpers<T>::get_precision(this->x)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T, int>()(0);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term of the expansion: x / ((n+1) * (n+2))
        state = utils::cast<T, K>()(n, utils::helpers<T>::get_precision(this->x));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
