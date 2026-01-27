#ifndef MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#define MINUS_3_DIV_4_OR_X_MINUS_3_DIV_4_ITERATOR_HPP
#pragma once

#include <numbers>

#include "../series_base.hpp"

/**
 * @file minus_3_div_4_or_x_minus_3_div_4_iterator.hpp
 * @brief Iterator for the Fourier series expansion of a piecewise linear function.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks {
namespace series {

/**
 * @brief Fourier series iterator for the piecewise function f(x) = -0.75 if x < 0, else x - 0.75.
 *
 * This class implements the Fourier expansion for a specific saw-tooth like
 * piecewise linear function.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class minus_3_div_4_or_x_minus_3_div_4_iterator final : public series_base_succ<T, K> {
public:
    /**
     * @brief Default constructor for minus_3_div_4_or_x_minus_3_div_4_iterator.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     */
    explicit minus_3_div_4_or_x_minus_3_div_4_iterator(T x) : series_base_succ<T, K>(x) {
        if (this->is_invalid()) throw std::invalid_argument("Invalid series argument");
    }

    /**
     * @brief Retrieves the analytic sum of the series (the value of the piecewise function).
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The analytic sum.
     */
    T get_sum() const override {
        using float_type = real_of<T>::value;
        if constexpr (isComplexLike<T>::value) {
            if (this->x.real() <= utils::cast<float_type>(0)) return utils::cast<T>(-0.75);
        } else {
            if (this->x <= utils::cast<T>(0)) return utils::cast<T>(-0.75);
        }

        return this->x - utils::cast<T>(0.75);
    }

    /**
     * @brief Validates the current evaluation point x.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return true if |x| >= 3 or non-finite, false otherwise.
     */
    bool is_invalid() const override {
        using float_type = real_of<T>::value;
        return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(3.0);
    }

    /**
     * @brief Computes the next term in the Fourier expansion of the piecewise function.
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @return T The next term of the series.
     */
    T next(K n, T& state) const override {
        const T piDiv3 = utils::cast<T>(std::numbers::pi) / utils::cast<T>(3);
        const T shifted_x = utils::cast<T>(n + 1) * piDiv3 * this->x;

        // Fourier series term formula involving both sine and cosine components
        state =
            utils::cast<T>(-2) /
                (piDiv3 * piDiv3 *
                 utils::cast<T>(3 * utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) *
                                utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)))) *
                utils::cos(shifted_x) +
            utils::minus_one_raised_to_power_n<T, K>(n) / (piDiv3 * utils::cast<T>(n + 1)) * utils::sin(shifted_x);
        return state;
    }
};

}  // namespace series
}  // namespace shanks
#endif
