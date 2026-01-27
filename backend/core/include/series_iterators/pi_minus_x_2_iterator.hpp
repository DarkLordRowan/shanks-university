#ifndef PI_MIN_X_2_ITERATOR_HPP
#define PI_MIN_X_2_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_minus_x_2_iterator.hpp
 * @brief Iterator for the Fourier series expansion of (pi - x) / 2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the linear function f(x) = (pi - x) / 2.
 *
 * This class implements the Fourier expansion for the given linear function,
 * which converges for values of x such that 0 < x < pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_minus_x_2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_minus_x_2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_minus_x_2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series ((pi - x) / 2).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of (pi - x) / 2.
     */
    T get_sum() const override { return (utils::cast<T>(std::numbers::pi) - this->x) * utils::cast<T>(0.5); }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is outside (0, pi) or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = GetUnderlyingType<T>::value;

        if constexpr (isComplexLike<T>::value) {
            return !utils::isfinite(this->x) || this->x.real() <= utils::cast<float_type>(0) ||
                   this->x.real() >= utils::cast<float_type>(std::numbers::pi);
        } else {
            return !utils::isfinite(this->x) || this->x <= utils::cast<T>(0) ||
                   this->x >= utils::cast<T>(std::numbers::pi);
        }
    }

    /**
     * @brief Computes the next term in the Fourier expansion of (pi - x) / 2.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series (sin((n+1)x) / (n+1)).
     */
    T next(K n, T& state) const override {
        // Fourier series term: sin(nx) / n for n >= 1
        state = utils::sin(utils::cast<T>(n + 1) * this->x) / utils::cast<T>(n + 1);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
