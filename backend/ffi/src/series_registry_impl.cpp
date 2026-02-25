/**
 * @file series_registry_impl.cpp
 * @brief Explicit template instantiations + static member definitions for
 *        series_registry<T, K>.
 *
 * On MinGW (Windows), static local variables inside inline function templates
 * are not reliably deduplicated when building a shared DLL — the linker
 * reports undefined references to the static-local storage symbols.
 *
 * Fix:
 *   - series_registry::s_entries is a static DATA MEMBER (not a local).
 *   - The one canonical definition is provided here via explicit template
 *     instantiation for types that are used across multiple TUs.
 *   - Every other TU sees `extern template class series_registry<T,K>`
 *     (via the SHANKS_FFI_SHARED_BUILD guard in series.hpp) and therefore
 *     does not emit its own copy.
 *
 * NOTE: std::complex<intprec::interval<T>> types are intentionally excluded
 * here because their iterator classes don't have full type info for all series,
 * which causes linker errors when forcing full class template instantiation.
 * Since those types are only used in series_ffi.cpp (a single TU), they have
 * no deduplication problem and use implicit instantiation normally.
 */

// Undef SHANKS_FFI_SHARED_BUILD so that series.hpp does NOT emit
// `extern template` in *this* translation unit — we need FULL definitions here.
#ifdef SHANKS_FFI_SHARED_BUILD
#undef SHANKS_FFI_SHARED_BUILD
#endif

#include "../../core/include/lib.hpp"

// ---------------------------------------------------------------------------
// Macro to build the entries vector element for one series (snake, camel).
// Mirrors the lambda in the original get_entries() body exactly.
// ---------------------------------------------------------------------------
#define MAKE_ENTRY(T_TYPE, K_TYPE, snake, camel)                                                        \
    {camel,                                                                                             \
     [](T_TYPE x, T_TYPE t, K_TYPE k) -> std::unique_ptr<shanks::series::series_base<T_TYPE, K_TYPE>> {\
         if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T_TYPE, K_TYPE>,        \
                                               T_TYPE, T_TYPE, K_TYPE>)                                 \
             return std::make_unique<shanks::series::snake##_iterator<T_TYPE, K_TYPE>>(x, t, k);        \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T_TYPE, K_TYPE>,   \
                                                    T_TYPE, T_TYPE>)                                    \
             return std::make_unique<shanks::series::snake##_iterator<T_TYPE, K_TYPE>>(x, t);           \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T_TYPE, K_TYPE>,   \
                                                    T_TYPE, K_TYPE>)                                    \
             return std::make_unique<shanks::series::snake##_iterator<T_TYPE, K_TYPE>>(x, k);           \
         else                                                                                           \
             return std::make_unique<shanks::series::snake##_iterator<T_TYPE, K_TYPE>>(x);              \
     },                                                                                                 \
     std::is_constructible_v<shanks::series::snake##_iterator<T_TYPE, K_TYPE>, T_TYPE, T_TYPE>,         \
     std::is_constructible_v<shanks::series::snake##_iterator<T_TYPE, K_TYPE>, T_TYPE, K_TYPE>},

// Build the entries initializer list for a given (T, K) by expanding the
// .def file with MAKE_ENTRY adapted for those types.
#define BUILD_ENTRIES(T_TYPE, K_TYPE)                                                                   \
    {                                                                                                   \
        _Pragma("GCC diagnostic push")                                                                  \
        _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")                                         \
        using _T = T_TYPE; using _K = K_TYPE;                                                           \
        _Pragma("GCC diagnostic pop")                                                                   \
    }

// We can't use _Pragma inside an initializer, so instead we define s_entries
// per specialization using a helper lambda that expands the .def.

// ---------------------------------------------------------------------------
// Macro to define s_entries for one (T, K) specialization.
// ---------------------------------------------------------------------------
#define DEFINE_S_ENTRIES(T_TYPE, K_TYPE)                                                                \
template <>                                                                                             \
const std::vector<shanks::series::series_registry<T_TYPE, K_TYPE>::entry>                              \
shanks::series::series_registry<T_TYPE, K_TYPE>::s_entries = [] {                                      \
    using T = T_TYPE;                                                                                   \
    using K = K_TYPE;                                                                                   \
    std::vector<shanks::series::series_registry<T_TYPE, K_TYPE>::entry> v;                             \
    v.reserve(OVERALL_SERIES_DEFINED);                                                                  \
                                                                                                        \
    auto add = [&](shanks::series::series_registry<T_TYPE, K_TYPE>::entry e) { v.push_back(std::move(e)); }; \
                                                                                                        \
    (void)add; /* suppress unused warning if somehow no entries */                                      \
                                                                                                        \
    /* Expand series_registry.def: each macro pushes one entry.           */\
    /* We redefine the SERIES_ENTRY family to call add() with the entry.  */\
    /* This mirrors the original initializer list, using if constexpr     */\
    /* to handle non-constructible combinations gracefully.               */\
                                                                                                        \
    auto make = [](const char* name,                                                                    \
                   shanks::series::series_registry<T_TYPE,K_TYPE>::IteratorFactory f,                   \
                   bool needsT, bool needsK)                                                            \
        -> shanks::series::series_registry<T_TYPE,K_TYPE>::entry                                        \
    { return {name, std::move(f), needsT, needsK}; };                                                  \
    (void)make;                                                                                         \
                                                                                                        \
    return v;                                                                                           \
}()

// The above approach also doesn't work cleanly with the .def expansion.
// Instead, use the simplest correct pattern: define s_entries directly
// using the same #include trick as the original class body, but for each type
// we do it as a non-template specialization.
//
// We undef/redef the macros for each use of the .def file.

#undef DEFINE_S_ENTRIES
#undef MAKE_ENTRY

// ---------------------------------------------------------------------------
// For each type specialization, define s_entries by including series_registry.def
// with appropriate SERIES_ENTRY macros that build entries for that exact type.
// ---------------------------------------------------------------------------
#define IMPL_S_ENTRIES(T_TYPE, K_TYPE) \
template <> \
const std::vector<shanks::series::series_registry<T_TYPE, K_TYPE>::entry> \
shanks::series::series_registry<T_TYPE, K_TYPE>::s_entries = [] { \
    using T [[maybe_unused]] = T_TYPE; \
    using K [[maybe_unused]] = K_TYPE; \
    using Entry = shanks::series::series_registry<T_TYPE, K_TYPE>::entry; \
    std::vector<Entry> _v; \
    _v.reserve(OVERALL_SERIES_DEFINED); \
    auto _add = [&](Entry e) { _v.push_back(std::move(e)); }; \
    (void)_add; \
    _add({"placeholder", nullptr, false, false}); \
    return _v; \
}()

// The macro approach is getting complex. Let's just use the direct approach:
// define s_entries for each type using the same macro expansion as in
// series.hpp's get_entries(), but as a free variable initializer.
// We define SERIES_ENTRY etc. to produce an entry, then #include the .def.

#undef IMPL_S_ENTRIES

// Final approach: define a helper macro SHANKS_IMPL_ENTRIES(T,K) that
// expands to the full braced initializer list for s_entries.
// This works because we're at namespace scope.

#define SERIES_ENTRY(snake, camel) \
    {camel, \
     [](T x, T t, K k) -> std::unique_ptr<shanks::series::series_base<T, K>> { \
         if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T, K>) \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, t, k); \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T>) \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, t); \
         else if constexpr (std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, K>) \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x, k); \
         else \
             return std::make_unique<shanks::series::snake##_iterator<T, K>>(x); \
     }, \
     std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, T>, \
     std::is_constructible_v<shanks::series::snake##_iterator<T, K>, T, K>},
#define SERIES_LAST(snake, camel) SERIES_ENTRY(snake, camel)
#define SERIES_ENTRY_ARGS(snake, camel, a1, a2) SERIES_ENTRY(snake, camel)
#define SERIES_LAST_ARGS(snake, camel, a1, a2) SERIES_LAST(snake, camel)

// Macro: instantiate s_entries for series_registry<T_TYPE, K_TYPE>.
// The 'using T/K' brings T and K into scope so the SERIES_ENTRY lambda
// can capture them as the template parameters.
#define INST_S_ENTRIES(T_TYPE, K_TYPE) \
template <> \
const std::vector<shanks::series::series_registry<T_TYPE, K_TYPE>::entry> \
shanks::series::series_registry<T_TYPE, K_TYPE>::s_entries = [] { \
    using T [[maybe_unused]] = T_TYPE; \
    using K [[maybe_unused]] = K_TYPE; \
    return std::vector<shanks::series::series_registry<T_TYPE, K_TYPE>::entry>{ \
        ENTRIES_LIST \
    }; \
}()

// We can't reference ENTRIES_LIST that way — the macros are expanded before
// they're defined. Use the #include directly in the lambda body instead.
// Since lambdas can contain #include directives (they're just text), this
// should be fine:
#undef INST_S_ENTRIES

// FINAL, WORKING approach: define s_entries for each type by using
// a free function with the .def expansion. The T and K aliases in the
// enclosing scope are picked up by SERIES_ENTRY's lambda via capture.

namespace shanks { namespace series {

// ---------------------------------------------------------------------------
// float, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<float, size_t>::entry>
series_registry<float, size_t>::s_entries = [] {
    using T [[maybe_unused]] = float;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<float, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// double, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<double, size_t>::entry>
series_registry<double, size_t>::s_entries = [] {
    using T [[maybe_unused]] = double;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<double, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// long double, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<long double, size_t>::entry>
series_registry<long double, size_t>::s_entries = [] {
    using T [[maybe_unused]] = long double;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<long double, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// mpfr::mpreal, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<mpfr::mpreal, size_t>::entry>
series_registry<mpfr::mpreal, size_t>::s_entries = [] {
    using T [[maybe_unused]] = mpfr::mpreal;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<mpfr::mpreal, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// std::complex<float>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<std::complex<float>, size_t>::entry>
series_registry<std::complex<float>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = std::complex<float>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<std::complex<float>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// std::complex<double>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<std::complex<double>, size_t>::entry>
series_registry<std::complex<double>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = std::complex<double>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<std::complex<double>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// std::complex<long double>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<std::complex<long double>, size_t>::entry>
series_registry<std::complex<long double>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = std::complex<long double>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<std::complex<long double>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// intprec::interval<float>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<intprec::interval<float>, size_t>::entry>
series_registry<intprec::interval<float>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = intprec::interval<float>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<intprec::interval<float>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// intprec::interval<double>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<intprec::interval<double>, size_t>::entry>
series_registry<intprec::interval<double>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = intprec::interval<double>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<intprec::interval<double>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// intprec::interval<long double>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<intprec::interval<long double>, size_t>::entry>
series_registry<intprec::interval<long double>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = intprec::interval<long double>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<intprec::interval<long double>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// ---------------------------------------------------------------------------
// intprec::interval<mpfr::mpreal>, size_t
// ---------------------------------------------------------------------------
template <>
const std::vector<series_registry<intprec::interval<mpfr::mpreal>, size_t>::entry>
series_registry<intprec::interval<mpfr::mpreal>, size_t>::s_entries = [] {
    using T [[maybe_unused]] = intprec::interval<mpfr::mpreal>;
    using K [[maybe_unused]] = size_t;
    return std::vector<series_registry<intprec::interval<mpfr::mpreal>, size_t>::entry>{
#include "../../core/include/series_registry.def"
    };
}();

// NOTE: std::complex<intprec::interval<T>> specializations are intentionally
// NOT listed here. Those types are only instantiated in series_ffi.cpp (a
// single TU), so there's no COMDAT/deduplication problem. Their s_entries
// static data member will be implicitly defined on first use — which works
// correctly since there's only one translation unit that triggers it.
// Forcing explicit instantiation for those types causes typeinfo linker errors
// because many series iterators can't be constructed for complex-interval types.

}} // namespace shanks::series

#undef SERIES_ENTRY
#undef SERIES_LAST
#undef SERIES_ENTRY_ARGS
#undef SERIES_LAST_ARGS

// ---------------------------------------------------------------------------
// Explicit template class instantiations (for all non-CInterval methods).
// These ensure that methods like create() are compiled into this TU.
// ---------------------------------------------------------------------------
namespace shanks { namespace series {

template class series_registry<float,       size_t>;
template class series_registry<double,      size_t>;
template class series_registry<long double, size_t>;
template class series_registry<mpfr::mpreal, size_t>;

template class series_registry<std::complex<float>,       size_t>;
template class series_registry<std::complex<double>,      size_t>;
template class series_registry<std::complex<long double>, size_t>;

template class series_registry<intprec::interval<float>,       size_t>;
template class series_registry<intprec::interval<double>,      size_t>;
template class series_registry<intprec::interval<long double>, size_t>;
template class series_registry<intprec::interval<mpfr::mpreal>, size_t>;

}} // namespace shanks::series
