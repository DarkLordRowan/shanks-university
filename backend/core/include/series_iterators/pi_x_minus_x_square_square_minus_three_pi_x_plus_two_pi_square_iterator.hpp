#ifndef PI_X_MINUS_X_SQUARE_SQUARE_MINUS_THREE_PI_X_PLUS_TWO_PI_SQUARE_ITERATOR_HPP
#define PI_X_MINUS_X_SQUARE_SQUARE_MINUS_THREE_PI_X_PLUS_TWO_PI_SQUARE_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a periodic parabolic function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the piecewise parabolic function:
 * f(x) = pi*x - x^2 for x in [0, pi] and f(x) = x^2 - 3*pi*x + 2*pi^2 for x in [pi, 2*pi].
 *
 * This class implements the Fourier sine expansion for the given periodic
 * parabolic segments, which converges for values of x such that 0 < x < 2*pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (the piecewise parabolic value).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the piecewise function at current point x.
     */
    T get_sum() const override {
        using float_type = GetUnderlyingType<T>::value;

        if constexpr (isComplexLike<T>::value) {
            if (this->x.real() <= utils::cast<float_type>(std::numbers::pi))
                return utils::cast<T>(std::numbers::pi) * this->x - this->x * this->x;
        } else {
            if (this->x <= utils::cast<T>(std::numbers::pi))
                return utils::cast<T>(std::numbers::pi) * this->x - this->x * this->x;
        }

        return this->x * this->x - utils::cast<T>(3.0 * std::numbers::pi) * this->x +
               utils::cast<T>(2.0 * std::numbers::pi) * utils::cast<T>(std::numbers::pi);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is outside [0, 2pi] or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;

        if constexpr (isComplexLike<T>::value) {
            return !utils::isfinite(this->x) || this->x.real() < utils::cast<float_type>(0) ||
                   this->x.real() > utils::cast<float_type>(2.0 * std::numbers::pi);
        } else {
            return !utils::isfinite(this->x) || this->x < utils::cast<T>(0) ||
                   this->x > utils::cast<T>(2.0 * std::numbers::pi);
        }
    }

    /**
     * @brief Computes the next term in the Fourier expansion of the parabolic function.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (8 * sin((2n+1)x) / (pi * (2n+1)^3)).
     */
    T next(K n, T& state) const override {
        // Formula for the odd harmonics of the Fourier sine series
        state = utils::cast<T>(8) *
                utils::sin(
                    utils::cast<T>(utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1))) *
                    this->x) /
                (utils::cast<T>(std::numbers::pi) *
                 utils::cast<T>(utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                                utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                                utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1))));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
