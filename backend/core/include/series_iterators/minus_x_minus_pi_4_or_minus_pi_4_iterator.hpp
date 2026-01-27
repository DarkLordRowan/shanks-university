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

namespace shanks
{
    namespace series
    {

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
        class minus_x_minus_pi_4_or_minus_pi_4_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for minus_x_minus_pi_4_or_minus_pi_4_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit minus_x_minus_pi_4_or_minus_pi_4_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (the value of the piecewise function).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of the piecewise function at x.
             */
            T get_sum() const override
            {
                using float_type = GetUnderlyingType<T>::value;

                if constexpr (isComplexLike<T>::value)
                {
                    if (this->x.real() <= utils::cast<float_type>(0))
                        return utils::cast<T>(-1) * this->x;
                }
                else
                {
                    if (this->x <= utils::cast<T>(0))
                        return utils::cast<T>(-1) * this->x;
                }

                return utils::cast<T>(0);
            }

            /**
             * @brief Validates the current evaluation point x.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return true if |x| >= pi or non-finite, false otherwise.
             */
            bool is_invalid() const override
            {
                using float_type = GetUnderlyingType<T>::value;
                return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(std::numbers::pi);
            }

            /**
             * @brief Computes the next term in the Fourier expansion of the piecewise function.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                // First term is the DC component (constant term)
                if (n == 0)
                    state = utils::cast<T>(std::numbers::pi * 0.25, utils::get_precision(state));
                else
                {
                    const size_t precision = utils::get_precision(state);
                    const T piDiv3 = utils::cast<T>(std::numbers::pi, precision) / utils::cast<T>(3, precision);
                    const T cos_coef = (utils::minus_one_raised_to_power_n<T, K>(n) - utils::cast<T>(1, precision)) /
                                       (utils::cast<T>(std::numbers::pi, precision) * utils::cast<T>(n * n, precision));
                    const T sin_coef = utils::minus_one_raised_to_power_n<T, K>(n) / utils::cast<T>(n, precision);
                    state = cos_coef * utils::cos(utils::cast<T>(n) * this->x) +
                            sin_coef * utils::sin(utils::cast<T>(n) * this->x);
                }
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
