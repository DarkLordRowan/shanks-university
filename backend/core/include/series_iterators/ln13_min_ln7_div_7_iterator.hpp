#ifndef LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#define LN13_MIN_LN7_DIV_7_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln13_min_ln7_div_7_iterator.hpp
 * @brief Iterator for the series expansion related to x * (ln(13) - ln(7)) / 7.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * ln(13/7) / 7.
 *
 * This class implements a specific series expansion whose analytic sum is
 * x * ln(13/7) / 7.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln13_min_ln7_div_7_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln13_min_ln7_div_7_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ln13_min_ln7_div_7_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * ln(13/7) / 7).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * ln(13/7) / 7.
     */
    T get_sum() const override {
        return this->x * utils::math<T>::log(utils::cast<T, int>()(13) / utils::cast<T, int>()(7)) /
               utils::cast<T, int>()(7);
    }

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
        // Specific term recurrence for the ln(13/7) related series
        if (n == 0)
            state = utils::cast<T, int>()(6) * this->x / utils::cast<T, int>()(49);
        else
            state *= utils::cast<T, int>()(-1) * utils::cast<T, K>()(6 * n) /
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(7), static_cast<size_t>(n),
                                                                       static_cast<size_t>(7)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
