#ifndef LN_COSX_ITERATOR_HPP
#define LN_COSX_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file ln_cosx_iterator.hpp
 * @brief Iterator for the series expansion of ln(cos(x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the natural logarithm of the cosine function ln(cos(x)).
 *
 * This class implements a specific series expansion for ln(cos(x)), which converges
 * for values of x such that |x| < pi/2.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln_cosx_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln_cosx_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ln_cosx_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (ln(cos(x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of ln(cos(x)).
     */
    T get_sum() const override { return utils::math<T>::log(utils::math<T>::cos(this->x)); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= pi/2 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) >= utils::cast<float_type, double>()(std::numbers::pi * 0.5);
    }

    /**
     * @brief Computes the next term in the series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence logic for the expansion terms involving sin(x)^2
        if (n == 0)
            state = utils::math<T>::sin(this->x) * utils::math<T>::sin(this->x) * utils::cast<T, double>()(-0.5);
        else
            state *= utils::math<T>::sin(this->x) * utils::math<T>::sin(this->x) * utils::cast<T, K>()(n) /
                     utils::cast<T, K>()(n + 1);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
