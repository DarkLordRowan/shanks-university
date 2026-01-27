#ifndef X_TWO_THROUGHT_SQUARES_ITERATOR_HPP
#define X_TWO_THROUGHT_SQUARES_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file x_two_throught_squares_iterator.hpp
 * @brief Iterator for a specific series expansion summing to x/2.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks
{
    namespace series
    {

        /**
         * @brief Series iterator for the function f(x) = x / 2 using a specific expansion.
         *
         * This class implements a specific series expansion whose analytic sum equals
         * x / 2, with terms involving higher-order powers in the denominator.
         *
         * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
         * @tparam T Floating-point type for series elements (AcceptedLike).
         * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
         */
        template <AcceptedLike T, UnsignedIntLike K>
        class x_two_throught_squares_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for x_two_throught_squares_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit x_two_throught_squares_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (x / 2).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of x / 2.
             */
            T get_sum() const override
            {
                return this->x * utils::cast<T>(0.5);
            }

            /**
             * @brief Validates the current evaluation point x.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return true if x is non-finite, false otherwise.
             */
            bool is_invalid() const override
            {
                return !utils::isfinite(this->x);
            }

            /**
             * @brief Computes the next term in the series expansion.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                // Specific term formula for this variant of the x/2 expansion
                state = utils::cast<T>(2 * (n + 1) * (n + 1) - 1) * this->x /
                        utils::cast<T>(4 * utils::pow(static_cast<size_t>(n + 1), size_t{4}) + 1);
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
