#ifndef CI_X_ITERATOR_HPP
#define CI_X_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file ci_x_iterator.hpp
 * @brief Iterator for the series expansion of the Cosine Integral Ci(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Series iterator for the Cosine Integral function Ci(x).
 *
 * This class implements the expansion of Ci(x) around zero, which includes
 * logarithmic and constant terms (Euler-Mascheroni constant) followed by
 * an infinite power series.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ci_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ci_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ci_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (Ci(x)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of Ci(x).
     */
    T get_sum() const override {
        if constexpr (typename utils::math<T>::has_ci_x{})
            return utils::math<T>::ci_x(this->x);

        assert(false);
        return utils::helpers<T>::get_nan();
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the Ci(x) expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First few terms are special (constant and logarithmic), followed by the power series
        if (n == 0)
            state = utils::cast<T, double>()(std::numbers::egamma, utils::helpers<T>::get_precision(state));
        else if (n == 1)
            state = utils::math<T>::log(this->x);
        else if (n == 2)
            state = this->x * this->x * utils::cast<T, double>()(-0.25);
        else
            state *=
                utils::cast<T, int>()(-1) * this->x * this->x * utils::cast<T, K>()(n - 2) /
                utils::cast<T, size_t>()(2 * (n - 1) * (n - 1) *
                                         utils::math<size_t>::fma(static_cast<size_t>(2), static_cast<size_t>(n - 2),
                                                                  static_cast<size_t>(1)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
