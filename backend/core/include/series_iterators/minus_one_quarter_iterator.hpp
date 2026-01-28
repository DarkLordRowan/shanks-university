#ifndef MINUS_ONE_QUARTER_ITERATOR_HPP
#define MINUS_ONE_QUARTER_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file minus_one_quarter_iterator.hpp
 * @brief Iterator for the series expansion related to -0.25 * x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = -0.25 * x.
 *
 * This class implements a specific series expansion whose analytic sum is
 * -0.25 * x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class minus_one_quarter_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for minus_one_quarter_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit minus_one_quarter_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (-0.25 * x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of -0.25 * x.
     */
    T get_sum() const override { return this->x * utils::cast<T>::meta(-0.25); }

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
        // Formula for the n-th term: (-1)^(n+1) * x / ((n+1) * (n+3))
        state = utils::minus_one_raised_to_power_n<T, K>(n + 1) * this->x / utils::cast<T>::meta((n + 1) * (n + 3));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
