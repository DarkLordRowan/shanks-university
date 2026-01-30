#ifndef PI_FOUR_MINUS_LN2_HALFED_ITERATOR_HPP
#define PI_FOUR_MINUS_LN2_HALFED_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_four_minus_ln2_halfed_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x * (pi/4 - ln(2)/2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the function f(x) = x * (pi/4 - ln(2) / 2).
 *
 * This class implements an alternating series expansion whose sum is
 * x * (pi/4 - ln(2) / 2).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_four_minus_ln2_halfed_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_four_minus_ln2_halfed_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_four_minus_ln2_halfed_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x * (pi/4 - ln(2) / 2).
     */
    T get_sum() const override {
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        return this->x * (utils::cast<T, double>()(std::numbers::pi * 0.25, precision) -
                          utils::math<T>::log(utils::cast<T, int>()(2, precision)) * utils::cast<T, double>()(0.5));
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
     * @brief Computes the next term in the alternating series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Alternating term formula: (-1)^n * x * (1/(2n+1) - 1/(2n+2))
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        state = utils::math<T>::template minus_one_raised_to_power_n<K>(n) * this->x *
                (utils::cast<T, int>()(1, precision) /
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                       static_cast<size_t>(1))) -
                 utils::cast<T, int>()(1, precision) /
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                       static_cast<size_t>(2))));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
