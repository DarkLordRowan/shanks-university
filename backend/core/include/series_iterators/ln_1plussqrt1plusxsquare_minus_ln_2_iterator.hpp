#ifndef LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#define LN_1PLUSSQRT1PLUSXSQUARE_MINUS_LN_2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln_1plussqrt1plusxsquare_minus_ln_2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1 + sqrt(1 + x^2)) - ln(2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1 + sqrt(1 + x^2)) - ln(2).
 *
 * This class implements the Maclaurin expansion of the given composite logarithmic
 * function, which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln_1plussqrt1plusxsquare_minus_ln_2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln_1plussqrt1plusxsquare_minus_ln_2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ln_1plussqrt1plusxsquare_minus_ln_2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (ln(1 + sqrt(1 + x^2)) - ln(2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the function at the current point x.
     */
    T get_sum() const override {
        return utils::math<T>::log(utils::cast<T, int>()(1) +
                                   utils::math<T>::sqrt(utils::cast<T, int>()(1) + this->x * this->x)) -
               utils::math<T>::log(utils::cast<T, int>()(2));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) >= utils::cast<float_type, int>()(1);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation for the Taylor expansion of ln(1 + sqrt(1 + x^2)) - ln(2)
        if (n == 0)
            state = this->x * this->x * utils::cast<T, double>()(0.25);
        else
            state *= utils::cast<T, int>()(-1) * this->x * this->x *
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                       static_cast<size_t>(1)) *
                                              n) /
                     utils::cast<T, K>()(2 * (n + 1) * (n + 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
