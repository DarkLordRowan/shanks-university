#ifndef SIN_ITERATOR_HPP
#define SIN_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file sin_iterator.hpp
 * @brief Iterator for the Taylor series expansion of the sine function sin(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the sine function sin(x).
 *
 * This class implements the Maclaurin expansion of sin(x), which converges
 * for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class sin_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for sin_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit sin_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (sin(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sin(x).
     */
    T get_sum() const override { return utils::math<T>::sin(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the sin(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence logic for the alternating odd-power terms of the sine expansion
        if (n == 0)
            state = this->x;
        else
            state *= utils::cast<T>::meta(-1.0) * this->x * this->x /
                     utils::cast<T>::meta(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                             static_cast<size_t>(1)) *
                                    2 * n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
