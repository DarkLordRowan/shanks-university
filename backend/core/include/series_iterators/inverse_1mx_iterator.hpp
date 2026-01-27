#ifndef INVERSE_1MX_ITERATOR_HPP
#define INVERSE_1MX_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file inverse_1mx_iterator.hpp
 * @brief Iterator for the geometric series expansion of 1/(1-x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 1 / (1 - x).
 *
 * This class implements the classic geometric series expansion, which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class inverse_1mx_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for inverse_1mx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit inverse_1mx_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (1 / (1 - x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 1 / (1 - x).
     */
    T get_sum() const override { return utils::cast<T>(1) / (utils::cast<T>(1) - this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the geometric series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (x^n).
     */
    T next(K n, T& state) const override {
        // Terms of the geometric series are powers of x
        if (n == 0)
            state = utils::cast<T>(1, utils::get_precision(state));
        else
            state *= this->x;
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
