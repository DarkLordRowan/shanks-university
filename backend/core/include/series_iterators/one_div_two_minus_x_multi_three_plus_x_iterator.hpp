#ifndef ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#define ONE_DIV_TWO_MINUS_X_MULTI_THREE_PLUS_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file one_div_two_minus_x_multi_three_plus_x_iterator.hpp
 * @brief Iterator for the series expansion of 1 / ((2 - x) * (3 + x)).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Taylor series iterator for the rational function f(x) = 1 / ((2 - x) * (3 + x)).
 *
 * This class implements the power series expansion of the given rational function,
 * obtained via partial fraction decomposition. It converges for values of x
 * such that |x| < 2.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class one_div_two_minus_x_multi_three_plus_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_div_two_minus_x_multi_three_plus_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit one_div_two_minus_x_multi_three_plus_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (1 / ((2 - x) * (3 + x))).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the rational function at x.
     */
    T get_sum() const override {
        return utils::cast<T, int>()(1) / ((utils::cast<T, int>()(2) - this->x) * (utils::cast<T, int>()(3) + this->x));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 2 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) >= utils::cast<float_type, int>()(2);
    }

    /**
     * @brief Computes the next term in the power series expansion.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Formula for the n-th term based on partial fraction decomposition components
        state =
            utils::math<T>::pow(this->x, utils::cast<T, K>()(n)) * utils::cast<T, double>()(0.2) *
            (utils::math<T>::template minus_one_raised_to_power_n<K>(n) *
                 utils::cast<T, size_t>()(
                     utils::math<size_t>::pow(static_cast<size_t>(2), static_cast<size_t>(n) + 1)) +
             utils::cast<T, size_t>()(utils::math<size_t>::pow(static_cast<size_t>(3), static_cast<size_t>(n) + 1))) /
            utils::cast<T, size_t>()(utils::math<size_t>::pow(static_cast<size_t>(6), static_cast<size_t>(n) + 1));

        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
