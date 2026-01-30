#ifndef LN_1PLUS_DIV_1PLUSX2_ITERATOR_HPP
#define LN_1PLUS_DIV_1PLUSX2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln_1plusx_div_1plusx2_iterator.hpp
 * @brief Iterator for the series expansion of ln(1+x) / (1+x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1+x) / (1+x^2).
 *
 * This class implements the expansion of the quotient of the natural logarithm
 * and a quadratic polynomial, which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln_1plusx_div_1plusx2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln_1plusx_div_1plusx2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ln_1plusx_div_1plusx2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (ln(1+x) / (1+x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of ln(1+x) / (1+x^2).
     */
    T get_sum() const override {
        return utils::math<T>::log(utils::cast<T, int>()(1) + this->x) / (utils::cast<T, int>()(1) + this->x * this->x);
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
        // Terms are generated based on the combined expansion logic for the quotient
        if (n == 0)
            state = this->x / (utils::cast<T, int>()(1) + this->x * this->x);
        else
            state *= utils::cast<T, int>()(-1) * this->x * utils::cast<T, K>()(n) / (utils::cast<T, K>()(n + 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
