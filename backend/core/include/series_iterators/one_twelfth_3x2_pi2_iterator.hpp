#ifndef ONE_TWELFTH_3X2_PI2_ITERATOR_HPP
#define ONE_TWELFTH_3X2_PI2_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file one_twelfth_3x2_pi2_iterator.hpp
 * @brief Iterator for the Fourier series expansion of (1/12)*(3*x^2 - pi^2).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the quadratic function f(x) = (3x^2 - pi^2) / 12.
 *
 * This class implements the Fourier expansion for the given parabolic function,
 * which is valid for x within the range [-pi, pi].
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class one_twelfth_3x2_pi2_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for one_twelfth_3x2_pi2_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit one_twelfth_3x2_pi2_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series ((3x^2 - pi^2) / 12).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the quadratic function at x.
     */
    T get_sum() const override {
        return utils::cast<T>::meta(1) / utils::cast<T>::meta(12) *
               (utils::cast<T>::meta(3) * this->x * this->x -
                utils::cast<T>::meta(std::numbers::pi) * utils::cast<T>::meta(std::numbers::pi));
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| > pi or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) > utils::cast<float_type>(std::numbers::pi);
    }

    /**
     * @brief Computes the next term in the Fourier expansion of the quadratic function.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // General Fourier term: (-1)^n * cos(nx) / n^2
        state = utils::minus_one_raised_to_power_n<T, K>(n + 1) * utils::math<T>::cos(utils::cast<T>::meta(n + 1) * this->x) /
                utils::cast<T>::meta((n + 1) * (n + 1));
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
