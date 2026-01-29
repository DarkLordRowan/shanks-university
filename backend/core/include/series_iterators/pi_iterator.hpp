#ifndef PI_ITERATOR_HPP
#define PI_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * pi.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the scaled constant function f(x) = x * pi.
 *
 * This class implements a specific series expansion whose analytic sum equals
 * x * pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * pi).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * pi.
     */
    T get_sum() const override { return this->x * utils::cast<T, double>()(std::numbers::pi); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T, int>()(0);
    }

    /**
     * @brief Computes the next term in the series expansion for x * pi.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term of the specific expansion
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        if (n == 0)
            state = this->x * utils::math<T>::sqrt(utils::cast<T, int>()(12, precision));
        else
            state *= utils::cast<T, int>()(-1, precision) *
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                                   static_cast<size_t>(1))) /
                     utils::cast<T, size_t>()(3 * utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                       static_cast<size_t>(1)));

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
