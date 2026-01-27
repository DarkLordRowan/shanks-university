#ifndef INVERSE_SQRT_1M4X_ITERATOR_HPP
#define INVERSE_SQRT_1M4X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file inverse_sqrt_1m4x_iterator.hpp
 * @brief Iterator for the series expansion of 1/sqrt(1-4x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 1 / sqrt(1 - 4x).
 *
 * This class implements the Maclaurin expansion of 1 / sqrt(1 - 4x), which
 * is related to the central binomial coefficients and converges for |x| < 0.25.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class inverse_sqrt_1m4x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for inverse_sqrt_1m4x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit inverse_sqrt_1m4x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (1 / sqrt(1 - 4x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 1 / sqrt(1 - 4x).
     */
    T get_sum() const override {
        return utils::cast<T>(1) / utils::sqrt(utils::cast<T>(1) - utils::cast<T>(4) * this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 0.25 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(0.25);
    }

    /**
     * @brief Computes the next term in the 1/sqrt(1-4x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Terms of this series are the central binomial coefficients C(2n, n) scaled by x^n
        if (n == 0)
            state = utils::cast<T>(1, utils::get_precision(state));
        else
            state *= this->x *
                     utils::cast<T>(
                         2 * utils::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1), static_cast<size_t>(1))) /
                     utils::cast<T>(n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
