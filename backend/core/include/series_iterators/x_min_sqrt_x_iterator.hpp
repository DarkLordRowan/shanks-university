#ifndef X_MIN_SQRT_X_ITERATOR_HPP
#define X_MIN_SQRT_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file x_min_sqrt_x_iterator.hpp
 * @brief Iterator for the series expansion of x - sqrt(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks
{
    namespace series
    {

        /**
         * @brief Series iterator for the function f(x) = x - sqrt(x).
         *
         * This class implements a specific series expansion for x - sqrt(x), typically
         * expanded around x=1, which converges for values of x such that |x - 1| < 1.
         *
         * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
         * @tparam T Floating-point type for series elements (AcceptedLike).
         * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
         */
        template <AcceptedLike T, UnsignedIntLike K>
        class x_min_sqrt_x_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for x_min_sqrt_x_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit x_min_sqrt_x_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (x - sqrt(x)).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of x - sqrt(x).
             */
            T get_sum() const override
            {
                return this->x - utils::sqrt(this->x);
            }

            /**
             * @brief Validates the current evaluation point x.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return true if |x - 1| >= 1 or x is non-finite, false otherwise.
             */
            bool is_invalid() const override
            {
                using float_type = GetUnderlyingType<T>::value;
                return !utils::isfinite(this->x) ||
                       utils::abs(this->x - utils::cast<T>(1)) >= utils::cast<float_type>(1.0);
            }

            /**
             * @brief Computes the next term in the series expansion of x - sqrt(x).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                if (n == 0)
                    state = this->x - utils::cast<T>(1);
                else if (n == 1)
                    state = (this->x - utils::cast<T>(1)) * utils::cast<T>(-0.5);
                else
                    state *= utils::cast<T>(-1) * (this->x - utils::cast<T>(1)) * utils::cast<T>(0.5) *
                             utils::cast<T>(2 * n - 3) / utils::cast<T>(n);
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
