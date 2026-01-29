#ifndef ARCSIN_X_MINUS_X_ITERATOR_HPP
#define ARCSIN_X_MINUS_X_ITERATOR_HPP

#pragma once

#include "../series_base.hpp"

/**
 * @file arcsin_x_minus_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arcsin(x) - x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = arcsin(x) - x.
 *
 * This class implements the Maclaurin expansion of arcsin(x) excluding the
 * first linear term, which converges for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class arcsin_x_minus_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for arcsin_x_minus_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit arcsin_x_minus_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (arcsin(x) - x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arcsin(x) - x.
     */
    T get_sum() const override { return utils::math<T>::asin(this->x) - this->x; }

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
     * @brief Computes the next term in the arcsin(x) - x Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // The expansion starts from the cubic term x^3 / 6
        if (n == 0)
            state = utils::math<T>::pow(this->x, utils::cast<T, int>()(3)) / utils::cast<T, int>()(6);
        else
            state *=
                this->x * this->x *
                utils::cast<T, size_t>()(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1))) /
                utils::cast<T, size_t>()(
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(2)) *
                    utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(3)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
