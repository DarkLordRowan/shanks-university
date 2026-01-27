#ifndef TEN_MINUS_X_ITERATOR_HPP
#define TEN_MINUS_X_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file ten_minus_x_iterator.hpp
 * @brief Iterator for the Fourier series expansion of the linear function f(x) = 10 - x.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the linear function f(x) = 10 - x.
 *
 * This class implements a specific Fourier expansion for the function 10 - x,
 * which is valid for values of x such that |x - 10| <= 5.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class ten_minus_x_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for ten_minus_x_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit ten_minus_x_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (10 - x).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of 10 - x.
     */
    T get_sum() const override { return utils::cast<T>(10) - this->x; }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x - 10| > 5 or x is non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x - utils::cast<T>(10)) > utils::cast<float_type>(5);
    }

    /**
     * @brief Computes the next term in the Fourier expansion of 10 - x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // Specific Fourier term formula for the linear segment expansion
        state = utils::cast<T>(10) * utils::minus_one_raised_to_power_n<T, K>(n + 1) /
                (utils::cast<T>(n + 1) * utils::cast<T>(std::numbers::pi)) *
                utils::sin(utils::cast<T>(n + 1) * utils::cast<T>(std::numbers::pi * 0.2) * this->x);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
