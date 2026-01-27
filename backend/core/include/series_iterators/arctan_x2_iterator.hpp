#ifndef ARCTAN_X2_ITERATOR_HPP
#define ARCTAN_X2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file arctan_x2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arctan(x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the inverse tangent function arctan(x^2).
 *
 * This class implements the Maclaurin expansion of arctan(x^2), which converges
 * for values of x such that |x| <= 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class arctan_x2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for arctan_x2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit arctan_x2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (arctan(x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arctan(x^2).
     */
    T get_sum() const override { return utils::atan(this->x * this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the arctan(x^2) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Each term is derived from the previous term using the Taylor formula for arctan(x^2)
        if (n == 0)
            state = this->x * this->x;
        else
            state *=
                utils::cast<T>(-1) * utils::pow(this->x, utils::cast<T>(4)) *
                utils::cast<T>(utils::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1), static_cast<size_t>(1))) /
                utils::cast<T>(utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
