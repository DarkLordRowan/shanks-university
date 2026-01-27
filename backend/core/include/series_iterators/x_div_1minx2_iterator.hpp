#ifndef X_DIV_1MINX2_ITERATOR_HPP
#define X_DIV_1MINX2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file x_div_1minx2_iterator.hpp
 * @brief Iterator for the series expansion of x / sqrt(1 - x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = x / sqrt(1 - x^2).
 *
 * This class implements the Maclaurin expansion of the given algebraic function,
 * which converges for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class x_div_1minx2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for x_div_1minx2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit x_div_1minx2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x / sqrt(1 - x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of x / sqrt(1 - x^2).
     */
    T get_sum() const override { return this->x / utils::sqrt(utils::cast<T>(1.0) - this->x * this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        if (n == 0)
            state = this->x;
        else
            state *=
                this->x * this->x *
                utils::cast<T>(utils::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1), static_cast<size_t>(1))) /
                utils::cast<T>(2 * n);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
