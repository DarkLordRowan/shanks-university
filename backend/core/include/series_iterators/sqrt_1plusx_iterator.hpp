#ifndef SQRT_1_PLUSX_ITERATOR_HPP
#define SQRT_1_PLUSX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file sqrt_1plusx_iterator.hpp
 * @brief Iterator for the Taylor series expansion of sqrt(1+x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the square root function f(x) = sqrt(1 + x).
 *
 * This class implements the generalized binomial expansion for sqrt(1+x),
 * which converges for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class sqrt_1plusx_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for sqrt_1plusx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit sqrt_1plusx_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (sqrt(1+x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sqrt(1 + x).
     */
    T get_sum() const override { return utils::math<T>::sqrt(utils::cast<T, int>()(1) + this->x); }

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
     * @brief Computes the next term in the binomial expansion of sqrt(1+x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation for binomial expansion with alpha = 0.5
        if (n == 0)
            state = utils::cast<T, int>()(1, utils::helpers<T>::get_precision(state));
        else
            state *= utils::cast<T, int>()(-1) * this->x * (utils::cast<T, K>()(2 * n) - utils::cast<T, int>()(3)) /
                     utils::cast<T, K>()(2 * n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
