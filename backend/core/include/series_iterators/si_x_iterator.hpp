#ifndef SI_X_ITERATOR_HPP
#define SI_X_ITERATOR_HPP
#pragma once

#include "../series_base.hpp"

/**
 * @file si_x_iterator.hpp
 * @brief Iterator for the series expansion of the Sine Integral Si(x).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks
{
    namespace series
    {

        /**
         * @brief Taylor series iterator for the Sine Integral function Si(x).
         *
         * This class implements the Maclaurin expansion of Si(x), which converges
         * for all finite values of x.
         *
         * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
         * @tparam T Floating-point type for series elements (AcceptedLike).
         * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
         */
        template <AcceptedLike T, UnsignedIntLike K>
        class si_x_iterator final : public series_base_succ<T, K>
        {
        public:
            /**
             * @brief Default constructor for si_x_iterator.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             */
            explicit si_x_iterator(T x) : series_base_succ<T, K>(x)
            {
                if (this->is_invalid())
                    throw std::invalid_argument("Invalid series argument");
            }

            /**
             * @brief Retrieves the analytic sum of the series (Si(x)).
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The value of the Sine Integral function at point x.
             */
            T get_sum() const override
            {
                return utils::si_x(this->x);
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
             * @brief Computes the next term in the Si(x) expansion.
             * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
             * @return T The next term of the series.
             */
            T next(K n, T& state) const override
            {
                // Formula for the n-th term involving iterated logarithms and powers
                if (n == 0)
                    state = this->x;
                else
                    state *=
                        utils::cast<T>(-1) * this->x * this->x *
                        utils::cast<T>(
                            utils::fma(static_cast<size_t>(2), static_cast<size_t>(n - 1), static_cast<size_t>(1))) /
                        utils::cast<T>(
                            utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) * 2 *
                            utils::fma(static_cast<size_t>(2), static_cast<size_t>(n), static_cast<size_t>(1)) * n);
                return state;
            }
        };

    } // namespace series
} // namespace shanks
#endif
