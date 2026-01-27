#ifndef SERIES_BASE_HPP
#define SERIES_BASE_HPP

#pragma once

#include <string>
#include <vector>

#include "custom_concepts.hpp"
#include "utils.hpp"

namespace shanks
{
    namespace series
    {

        /**
         * @file series_base.hpp
         * @brief Base class for all series implementations.
         */

        template <AcceptedLike T, UnsignedIntLike K>
        class series_base
        {
        public:
            series_base() = delete;
            explicit series_base(T x) : x(x)
            {
            }
            virtual ~series_base() = default;

            T get_x() const
            {
                return x;
            }
            virtual std::string get_name() const
            {
                return "unknown_series";
            }

            virtual T get_sum() const = 0;
            virtual series_result<T> generate(K n) = 0;
            virtual bool is_invalid() const = 0;

        protected:
            T x;

            template <typename IteratorFunc>
            static series_result<T> generate_from_iterator(K n, IteratorFunc func)
            {
                // sample for precision
                const T sample = func();

                std::vector<T> an(n, utils::cast<T>(0.0, utils::get_precision(sample)));
                std::vector<T> Sn(n, utils::cast<T>(0.0, utils::get_precision(sample)));

                an[0] += sample;
                Sn[0] += sample;
                for (K i = 1; i < n; ++i)
                {
                    an[i] += func();
                    Sn[i] += Sn[i - static_cast<K>(1)] + an[i];
                }

                return {Sn, an};
            }
        };

        template <AcceptedLike T, UnsignedIntLike K, typename State>
        class series_base_iter : public series_base<T, K>
        {
        public:
            explicit series_base_iter(T x) : series_base<T, K>(x)
            {
            }

            virtual T get_sum() const override = 0;
            virtual State initial_state() const
            {
                return State();
            };
            virtual T next(K index, State& state) const = 0;

            bool is_invalid() const override
            {
                return true;
            }

            series_result<T> generate(K n) override
            {
                State state = initial_state();

                return series_base<T, K>::generate_from_iterator(
                    n, [this, &state, i = K(0)]() mutable { return this->next(i++, state); });
            }
        };

        template <AcceptedLike T, UnsignedIntLike K>
        class series_base_succ : public series_base_iter<T, K, T>
        {
        public:
            explicit series_base_succ(T x) : series_base_iter<T, K, T>(x)
            {
            }

            virtual T get_sum() const override = 0;
            T initial_state() const
            {
                return utils::cast<T>(0.0, utils::get_precision(this->x));
            };
            virtual T next(K index, T& state) const = 0;

            bool is_invalid() const override
            {
                return true;
            }
        };

    } // namespace series
} // namespace shanks

#endif // SERIES_BASE_HPP
