#ifndef SERIES_HPP
#define SERIES_HPP
#include <type_traits>
#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "custom_concepts.hpp"
#include "series_base.hpp"
#include "series_iterators/series_iterators.hpp"

namespace shanks
{
    namespace series
    {

        /**
         * @brief Type-safe index for series in the registry.
         * Generated automatically from series_registry.def.
         */
        enum class series_id_t : size_t
        {
#define SERIES_ENTRY(snake, camel) snake##_id,
#define SERIES_LAST(snake, camel) snake##_id,
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) snake##_id,
#define SERIES_LAST_ARGS(snake, camel, a1, a2) snake##_id,
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
            series_id_t_count
        };

#define OVERALL_SERIES_DEFINED static_cast<size_t>(shanks::series::series_id_t::series_id_t_count)

        template <AcceptedLike T, UnsignedIntLike K>
        class series_registry
        {
        public:
            using IteratorFactory = std::function<std::unique_ptr<series_base<T, K>>(T, T, K)>;

            struct entry
            {
                std::string name;
                IteratorFactory factory;
                bool needsT;
                bool needsK;
            };

            static const std::vector<entry>& get_entries()
            {
                static const std::vector<entry> entries = {
// DEFININTION OF SERIES_ENTRY MACRO
#define SERIES_ENTRY(snake, camel)                                                                                     \
    {camel,                                                                                                            \
     [](T x, T t, K k) -> std::unique_ptr<series_base<T, K>> {                                                         \
         if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T, K>)                       \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, t, k);                                 \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T>)                     \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, t);                                    \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, K>)                     \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, k);                                    \
         else                                                                                                          \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x);                                       \
     },                                                                                                                \
     std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T>,                                            \
     std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, K>},
// DEFININTION OF SERIS_LAST MACRO
#define SERIES_LAST(snake, camel) SERIES_ENTRY(snake, camel)
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) SERIES_ENTRY(snake, camel)
#define SERIES_LAST_ARGS(snake, camel, a1, a2) SERIES_LAST(snake, camel)
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
                };
                return entries;
            }

            static std::unique_ptr<series_base<T, K>>
            create(size_t index, T x, T addTParameter = T(1), K addKParameter = K(1));
        };

        struct series_registry_metadata
        {
            static std::vector<std::string> get_names()
            {
                return {
#define SERIES_ENTRY(snake, camel) camel,
#define SERIES_LAST(snake, camel) camel
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) camel,
#define SERIES_LAST_ARGS(snake, camel, a1, a2) camel
#include "series_registry.def"
#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS
                };
            }
        };

        template <AcceptedLike T, UnsignedIntLike K>
        inline std::unique_ptr<series_base<T, K>>
        series_registry<T, K>::create(size_t index, T x, T addTParameter, K addKParameter)
        {
            const auto& entries = get_entries();
            if (index >= entries.size())
                throw std::out_of_range("Series index out of range");
            return entries[index].factory(x, addTParameter, addKParameter);
        }

    } // namespace series
} // namespace shanks

#endif
