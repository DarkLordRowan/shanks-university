#ifndef MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#define MINUS_X_MINUS_PI_4_OR_MINUS_PI_4_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file minus_x_minus_pi_4_or_minus_pi_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a specific piecewise function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = -x if -pi <= x < 0, else 0 if 0 <= x <= pi.
 *
 * This class implements the Fourier expansion for a half-rectified linear function.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class minus_x_minus_pi_4_or_minus_pi_4_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for minus_x_minus_pi_4_or_minus_pi_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit minus_x_minus_pi_4_or_minus_pi_4_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (the value of the piecewise function).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the piecewise function at x.
     */
    T get_sum() const override {
        using float_type = real_of<T>::value;

        if constexpr (isComplexLike<T>::value) {
            if (this->x.real() <= utils::cast<float_type, int>()(0)) return utils::cast<T, int>()(-1) * this->x;
        } else {
            if (this->x <= utils::cast<T, int>()(0)) return utils::cast<T, int>()(-1) * this->x;
        }

        return utils::cast<T, int>()(0);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= pi or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::helpers<T>::isfinite(this->x) ||
               utils::math<T>::abs(this->x) >= utils::cast<float_type, double>()(std::numbers::pi);
    }

    /**
     * @brief Computes the next term in the Fourier expansion of the piecewise function.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        // First term is the DC component (constant term)
        const size_t precision = utils::helpers<T>::get_precision(state);
        if (n == 0)
            state = utils::cast<T, double>()(std::numbers::pi * 0.25, precision);
        else {
            const T piDiv3 =
                utils::cast<T, double>()(std::numbers::pi, precision) / utils::cast<T, int>()(3, precision);
            const T cos_coef =
                (utils::math<T>::template minus_one_raised_to_power_n<K>(n) - utils::cast<T, int>()(1, precision)) /
                (utils::cast<T, double>()(std::numbers::pi, precision) * utils::cast<T, K>()(n * n, precision));
            const T sin_coef =
                utils::math<T>::template minus_one_raised_to_power_n<K>(n) / utils::cast<T, K>()(n, precision);
            state = cos_coef * utils::math<T>::cos(utils::cast<T, K>()(n) * this->x) +
                    sin_coef * utils::math<T>::sin(utils::cast<T, K>()(n) * this->x);
        }
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
