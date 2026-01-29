#ifndef SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#define SERIES_WITH_LN_NUMBER2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file series_with_ln_number2_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * 5.71617784132.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for a complex expansion summing to x * 5.71617784132.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * approximately 5.71617784132 * x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class series_with_ln_number2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for series_with_ln_number2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit series_with_ln_number2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the approximate analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 5.71617784132 * x.
     */
    T get_sum() const override { return utils::cast<T, double>()(5.71617784132) * this->x; }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the specific expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term involving iterated logarithms and powers
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        const T ln_n2 = utils::math<T>::log(utils::cast<T, K>()(n + 2, precision));
        state = this->x / utils::math<T>::pow(ln_n2, ln_n2);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
