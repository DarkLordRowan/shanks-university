#ifndef LN1_M_X2_ITERATOR_HPP
#define LN1_M_X2_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln1_m_x2_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1-x^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the function f(x) = ln(1 - x^2).
 *
 * This class implements the Maclaurin expansion of ln(1 - x^2), which converges
 * for values of x such that |x| < 1.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ln1_m_x2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ln1_m_x2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ln1_m_x2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (ln(1 - x^2)).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of ln(1 - x^2).
     */
    T get_sum() const override { return utils::log(utils::cast<T>(1) - this->x * this->x); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 1 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
    }

    /**
     * @brief Computes the next term in the ln(1-x^2) Taylor expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Recurrence relation derived from the ln(1-u) expansion with u = x^2
        if (n == 0)
            state = utils::cast<T>(-1) * this->x * this->x;
        else
            state *= this->x * this->x * utils::cast<T>(n) / utils::cast<T>(n + 1);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
