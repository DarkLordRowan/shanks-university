#ifndef COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#define COS3XMIB1_DIV_XSQUARE_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file cos3xmin1_div_xsquare_iterator.hpp
 * @brief Iterator for the series expansion of (cos(3x-1))/x^2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = (cos(3x-1))/x^2.
 *
 * This class implements the expansion of the shifted cosine function normalized
 * by x^2. The series is singular at x=0.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class cos3xmin1_div_xsquare_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for cos3xmin1_div_xsquare_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit cos3xmin1_div_xsquare_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series ((cos(3x-1))/x^2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (cos(3x-1))/x^2.
     */
    T get_sum() const override {
        return utils::math<T>::cos(utils::cast<T, int>()(3) * this->x - utils::cast<T, int>()(1)) / (this->x * this->x);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is zero or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        return !utils::helpers<T>::isfinite(this->x) || this->x == utils::cast<T, int>()(0);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term corresponds to the leading term of the expansion at n=0
        if (n == 0)
            state = utils::cast<T, int>()(1, utils::helpers<T>::get_precision(this->x)) / (this->x * this->x);
        else {
            const T var_3x_1squared =
                utils::math<T>::fma(utils::cast<T, int>()(3), this->x, utils::cast<T, int>()(-1)) *
                utils::math<T>::fma(utils::cast<T, int>()(3), this->x, utils::cast<T, int>()(-1));
            // Recursive update based on the Taylor series for cos(u) where u = 3x-1
            state *=
                utils::cast<T, int>()(-1) * var_3x_1squared /
                utils::cast<T, size_t>()(2 * n *
                                         utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                                  static_cast<size_t>(1)));
        }
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
