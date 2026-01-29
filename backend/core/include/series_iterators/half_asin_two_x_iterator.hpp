#ifndef HALF_ASIN_TWO_X_ITERATOR_HPP
#define HALF_ASIN_TWO_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file half_asin_two_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 0.5 * asin(2x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = 0.5 * asin(2x).
 *
 * This class implements the Maclaurin expansion of 0.5 * asin(2x), which converges
 * for values of x such that |x| <= 0.5.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class half_asin_two_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for half_asin_two_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit half_asin_two_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (0.5 * asin(2x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 0.5 * asin(2x).
     */
    T get_sum() const override {
        return utils::cast<T, int>()(0.5) * utils::math<T>::asin(utils::cast<T, int>()(2) * this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > 0.5 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) > utils::cast<float_type, double>()(0.5);
    }

    /**
     * @brief Computes the next term in the 0.5 * asin(2x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation derived from the arcsin expansion with substitution x -> 2x and scaling factor 0.5
        if (n == 0)
            state = this->x;
        else
            state *= this->x * this->x *
                     utils::cast<T, size_t>()(2 * n *
                                          utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                                   static_cast<size_t>(1)) *
                                          utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                                   static_cast<size_t>(1))) /
                     utils::cast<T, size_t>()(n * n *
                                          utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                                   static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
