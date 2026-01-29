#ifndef PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#define PI_MINUS_3PI_4_AND_PI_MINUS_X_MINUS_3PI_4_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a specific piecewise linear function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = pi/4 if x < 0, else pi/4 - x if x >= 0.
 *
 * This class implements the Fourier expansion for a specific piecewise linear
 * function, which converges for values of x such that |x| < pi.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (the piecewise function value).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The value of the piecewise function at current point x.
     */
    T get_sum() const override {
        using float_type = real_of<T>::value;

        if constexpr (isComplexLike<T>::value) {
            if (this->x.real() <= utils::cast<float_type, int>()(0)) return utils::cast<T, double>()(0.25 * std::numbers::pi);
        } else {
            if (this->x <= utils::cast<T, int>()(0)) return utils::cast<T, double>()(0.25 * std::numbers::pi);
        }

        return utils::cast<T, double>()(0.25 * std::numbers::pi) - this->x;
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
        // General Fourier term formula involving alternating cosine and sine components
        const size_t precision = utils::helpers<T>::get_precision(this->x);
        const K n1 = n + 1;
        state = utils::math<T>::cos(utils::cast<T, K>()(n1) * this->x) *
                    (utils::cast<T, int>()(1, precision) + utils::math<T>::template minus_one_raised_to_power_n<K>(n)) /
                    (utils::cast<T, double>()(std::numbers::pi) * utils::cast<T, K>()(n1 * n1)) +
                utils::math<T>::sin(utils::cast<T, K>()(n1) * this->x) * utils::math<T>::template minus_one_raised_to_power_n<K>(n1) /
                    utils::cast<T, K>()(n1);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
