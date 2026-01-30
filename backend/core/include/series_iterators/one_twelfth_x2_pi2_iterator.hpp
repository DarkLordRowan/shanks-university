#ifndef ONE_TWELFTH_X2_PI2_ITERATOR_HPP
#define ONE_TWELFTH_X2_PI2_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file one_twelfth_x2_pi2_iterator.hpp
 * @brief Iterator for the Fourier series expansion of x*(x^2 - pi^2)/12.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the cubic function f(x) = x * (x^2 - pi^2) / 12.
 *
 * This class implements the Fourier expansion for the given odd cubic function,
 * which converges for values of x within the range [-pi, pi].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class one_twelfth_x2_pi2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_twelfth_x2_pi2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit one_twelfth_x2_pi2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (x * (x^2 - pi^2) / 12).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the cubic function at point x.
     */
    T get_sum() const override {
        return this->x / utils::cast<T, int>()(12) *
               (this->x * this->x -
                utils::cast<T, double>()(std::numbers::pi) * utils::cast<T, double>()(std::numbers::pi));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if x is non-finite, false otherwise.
     */
    bool is_invalid() const override { return !utils::helpers<T>::isfinite(this->x); }

    /**
     * @brief Computes the next term in the Fourier expansion of the cubic function.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series ((-1)^(n+1) * sin((n+1)x) / (n+1)^3).
     */
    T next(K n, T& state) const override {
        // Formula for the n-th Fourier harmonic
        state = utils::math<T>::template minus_one_raised_to_power_n<K>(n + 1) *
                utils::math<T>::sin(utils::cast<T, K>()(n + 1) * this->x) /
                utils::cast<T, size_t>()(utils::math<size_t>::pow(static_cast<size_t>(n + 1), static_cast<size_t>(3)));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
