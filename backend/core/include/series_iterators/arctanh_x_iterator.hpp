#ifndef ARCTANH_X_ITERATOR_HPP
#define ARCTANH_X_ITERATOR_HPP

#pragma once

#include "../series_base.hpp"

/**
 * @file arctanh_x_iterator.hpp
 * @brief Iterator for the Taylor series expansion of arctanh(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the inverse hyperbolic tangent function arctanh(x).
 *
 * This class implements the Maclaurin expansion of arctanh(x), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class arctanh_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for arctanh_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit arctanh_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (arctanh(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of arctanh(x).
     */
    T get_sum() const override { return utils::math<T>::atanh(this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) || utils::math<T>::abs(this->x) > utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the arctanh(x) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Standard Taylor expansion for inverse hyperbolic tangent
        if (n == 0)
            state = this->x;
        else
            state *= this->x * this->x *
                     utils::cast<T>::meta(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1),
                                                             static_cast<size_t>(1))) /
                     utils::cast<T>::meta(utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n),
                                                             static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
