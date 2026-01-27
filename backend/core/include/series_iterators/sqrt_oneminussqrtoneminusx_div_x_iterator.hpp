#ifndef SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#define SQRT_ONEMINUSSQRTONEMINUSX_DIV_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file sqrt_oneminussqrtoneminusx_div_x_iterator.hpp
 * @brief Iterator for the series expansion of sqrt((1 - sqrt(1 - x)) / x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the composite function f(x) = sqrt((1 - sqrt(1 - x)) / x).
 *
 * This class implements the Maclaurin expansion of the given nested radical function,
 * which converges for values of x such that |x| < 1 (excluding x=0).
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class sqrt_oneminussqrtoneminusx_div_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for sqrt_oneminussqrtoneminusx_div_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit sqrt_oneminussqrtoneminusx_div_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of sqrt((1 - sqrt(1 - x)) / x).
     */
    T get_sum() const override {
        return utils::sqrt((utils::cast<T>(1) - utils::sqrt(utils::cast<T>(1) - this->x)) / this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1, x is zero, or x is non-finite; false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::isfinite(this->x) || this->x == utils::cast<T>(0) ||
               utils::abs(this->x) >= utils::cast<float_type>(1);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term corresponds to the limit at x -> 0, subsequent terms are calculated recursively
        if (n == 0)
            state = utils::cast<T>(1, utils::get_precision(state)) /
                    utils::sqrt(utils::cast<T>(2, utils::get_precision(state)));
        else
            state *=
                this->x *
                utils::cast<T>(utils::fma(static_cast<size_t>(4), static_cast<size_t>(n - 1), static_cast<size_t>(1)) *
                               utils::fma(static_cast<size_t>(4), static_cast<size_t>(n - 1), static_cast<size_t>(3))) /
                utils::cast<T>(8 * n *
                               utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
