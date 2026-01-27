#ifndef FOUR_ARCTAN_ITERATOR_HPP
#define FOUR_ARCTAN_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file four_arctan_iterator.hpp
 * @brief Iterator for the Taylor series expansion of 4*arctan(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks
{
    namespace series
    {

        /**
         * @brief Taylor series iterator for the function f(x) = 4 * arctan(x).
         *
         * This class implements the Maclaurin expansion of 4 * arctan(x), which converges
         * for values of x such that |x| <= 1.
         *
         * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
         * @tparam T Floating-point type for series elements (AcceptedLike).
         * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
         */
        template <AcceptedLike T, UnsignedIntLike K>
        class four_arctan_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for four_arctan_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit four_arctan_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (4 * arctan(x)).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of 4 * arctan(x).
             */
            T get_sum() const override
            {
                return utils::cast<T>(4) * utils::atan(this->x);
            }

            /**
             * @brief Validates the current evaluation point x.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return true if |x| > 1 or non-finite, false otherwise.
             */
            bool is_invalid() const override
            {
                using float_type = GetUnderlyingType<T>::value;
                return !utils::isfinite(this->x) || utils::abs(this->x) > utils::cast<float_type>(1.0);
            }

            /**
             * @brief Computes the next term in the 4 * arctan(x) Taylor expansion.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                // Recurrence logic for the alternating odd-power terms of the arctan expansion
                if (n == 0)
                    state = utils::cast<T>(4) * this->x;
                else
                    state *= utils::cast<T>(-1) * this->x * this->x * utils::cast<T>(2 * n - 1) /
                             utils::cast<T>(
                                 utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)));
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
