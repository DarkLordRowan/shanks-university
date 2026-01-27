#ifndef LN_1_PLUS_X3_ITERATOR_HPP
#define LN_1_PLUS_X3_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file ln_1_plus_x3_iterator.hpp
 * @brief Iterator for the Taylor series expansion of ln(1 + x^3).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks
{
    namespace series
    {

        /**
         * @brief Taylor series iterator for the function f(x) = ln(1 + x^3).
         *
         * This class implements the Maclaurin expansion of ln(1 + x^3), which converges
         * for values of x such that |x| < 1.
         *
         * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
         * @tparam T Floating-point type for series elements (AcceptedLike).
         * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
         */
        template <AcceptedLike T, UnsignedIntLike K>
        class ln_1_plus_x3_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for ln_1_plus_x3_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit ln_1_plus_x3_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (ln(1 + x^3)).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of ln(1 + x^3).
             */
            T get_sum() const override
            {
                return utils::log(utils::cast<T>(1) + utils::pow(this->x, utils::cast<T>(3)));
            }

            /**
             * @brief Validates the current evaluation point x.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return true if |x| >= 1 or non-finite, false otherwise.
             */
            bool is_invalid() const override
            {
                using float_type = GetUnderlyingType<T>::value;
                return !utils::isfinite(this->x) || utils::abs(this->x) >= utils::cast<float_type>(1.0);
            }

            /**
             * @brief Computes the next term in the ln(1+x^3) Taylor expansion.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                // Recurrence relation based on the ln(1+u) expansion where u = x^3
                if (n == 0)
                    state = utils::pow(this->x, utils::cast<T>(3.0));
                else
                    state *= utils::cast<T>(-1.0) * utils::pow(this->x, utils::cast<T>(3.0)) *
                             utils::cast<T>(static_cast<double>(n)) / utils::cast<T>(static_cast<double>(n + 1));
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
