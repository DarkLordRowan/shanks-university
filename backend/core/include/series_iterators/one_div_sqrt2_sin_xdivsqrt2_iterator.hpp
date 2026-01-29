#ifndef ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#define ONE_DIV_SQRT2_SIN_XDIVSQRT2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file one_div_sqrt2_sin_xdivsqrt2_iterator.hpp
 * @brief Iterator for the series expansion of (1/sqrt(2)) * sin(x/sqrt(2)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = (1/sqrt(2)) * sin(x/sqrt(2)).
 *
 * This class implements the Maclaurin expansion of the scaled sine function,
 * which converges for all finite values of x.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class one_div_sqrt2_sin_xdivsqrt2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_div_sqrt2_sin_xdivsqrt2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit one_div_sqrt2_sin_xdivsqrt2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series ((1/sqrt(2)) * sin(x/sqrt(2))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (1/sqrt(2)) * sin(x/sqrt(2)).
     */
    T get_sum() const override {
        return utils::cast<T, int>()(1) / utils::math<T>::sqrt(utils::cast<T, int>()(2)) *
               utils::math<T>::sin(this->x * utils::cast<T, int>()(1) / utils::math<T>::sqrt(utils::cast<T, int>()(2)));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the scaled sine Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation derived from the sin(u) expansion with u = x/sqrt(2)
        if (n == 0)
            state = this->x * utils::cast<T, double>()(0.5);
        else
            state *= utils::cast<T, int>()(-1) * this->x * this->x /
                     utils::cast<T, size_t>()(4 * n *
                                          utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                   static_cast<size_t>(1)));

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
