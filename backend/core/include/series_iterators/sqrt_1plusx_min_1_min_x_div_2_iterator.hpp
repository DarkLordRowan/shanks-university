#ifndef SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP
#define SQRT_1PLUSX_MIN_1_MIN_X_DIV_2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file sqrt_1plusx_min_1_min_x_div_2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of sqrt(1+x) - 1 - x/2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = sqrt(1 + x) - 1 - x/2.
 *
 * This class implements the Maclaurin expansion of sqrt(1+x) starting from the
 * quadratic term, which converges for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_min_1_min_x_div_2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for sqrt_1plusx_min_1_min_x_div_2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit sqrt_1plusx_min_1_min_x_div_2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (sqrt(1+x) - 1 - x/2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the truncated square root expansion at x.
     */
    T get_sum() const override {
        return utils::math<T>::sqrt(utils::cast<T, int>()(1) + this->x) - utils::cast<T, int>()(1) -
               this->x * utils::cast<T, double>()(0.5);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) > utils::cast<float_type, int>()(1);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        if (n == 0)
            state = this->x * this->x * utils::cast<T, double>()(-0.125);
        else
            state *= utils::cast<T, int>()(-1) * this->x *
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                   static_cast<size_t>(1))) /
                     utils::cast<T, size_t>()(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                   static_cast<size_t>(2)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
