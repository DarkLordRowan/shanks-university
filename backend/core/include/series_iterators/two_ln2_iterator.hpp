#ifndef TWO_LN2_ITERATOR_HPP
#define TWO_LN2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file two_ln2_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 2 * ln(2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * 2 * ln(2).
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * 2 * ln(2) (or x * ln(4)).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class two_ln2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for two_ln2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit two_ln2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * 2 * ln(2).
     */
    T get_sum() const override { return this->x * utils::log(utils::cast<T>(4)); }

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
        const K n1 = n + 1;
        // Formula for the n-th term of the specific series expansion
        state = utils::cast<T>(12 * n1 * n1 - 1) * this->x / utils::cast<T>(n1 * (4 * n1 * n1 - 1) * (4 * n1 * n1 - 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
