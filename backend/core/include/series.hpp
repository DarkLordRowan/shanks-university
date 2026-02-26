#ifndef SERIES_HPP
#define SERIES_HPP
#pragma once

#include <memory>
#include <functional>

namespace shanks {
namespace series {

inline bool is_seq(shanks::series::series_iterator_id_t id){
    return id == shanks::series::series_iterator_id_t::rump_seq1_iterator_id ||
           id == shanks::series::series_iterator_id_t::rump_seq2_iterator_id ||
           id == shanks::series::series_iterator_id_t::rump_seq3_iterator_id ||
           id == shanks::series::series_iterator_id_t::rump_seq4_iterator_id ||
           id == shanks::series::series_iterator_id_t::rump_seq5_iterator_id ||
           id == shanks::series::series_iterator_id_t::rump_seq6_iterator_id;
}

/**
 * @brief Type-safe index for series in the registry.
 * Generated automatically from series_registry.def.
 */
enum class series_id_t : size_t {
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
class series_registry {
public:
    using IteratorFactory = std::function<std::unique_ptr<series_base<T, K>>(T, T, K)>;

    struct entry {
        std::string name;
        IteratorFactory factory;
        bool needsT;
        bool needsK;
    };

    // Static data member — has proper external linkage on all platforms,
    // unlike a static local inside an inline function, which MinGW fails to
    // deduplicate across translation units when building a shared DLL.
    // Defined in series_registry_impl.cpp via explicit specialization.
    static const std::vector<entry> s_entries;

    static const std::vector<entry>& get_entries() {
        return s_entries;
    }

    static std::unique_ptr<series_base<T, K>> create(size_t index, T x, T addTParameter = T(1), K addKParameter = K(1));
};

struct series_registry_metadata {
    static std::vector<std::string> get_names() {
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
inline std::unique_ptr<series_base<T, K>> series_registry<T, K>::create(size_t index, T x, T addTParameter,
                                                                        K addKParameter) {
    const auto& entries = get_entries();
    if (index >= entries.size()) throw std::out_of_range("Series index out of range");
    return entries[index].factory(x, addTParameter, addKParameter);
}

}  // namespace series
}  // namespace shanks

// ---------------------------------------------------------------------------
// Explicit instantiation declarations (extern template).
// Suppresses implicit instantiation (and thus implicit definition of the
// s_entries static member) in every TU that includes this header when
// SHANKS_FFI_SHARED_BUILD is defined.  The single definition is provided in
// backend/ffi/src/series_registry_impl.cpp.
// ---------------------------------------------------------------------------
#ifdef SHANKS_FFI_SHARED_BUILD
#include "custom_types/intervalprecision.fwd.hpp"
#include "custom_types/mpreal.h"
#include <complex>

namespace shanks { namespace series {

extern template class series_registry<float,       size_t>;
extern template class series_registry<double,      size_t>;
extern template class series_registry<long double, size_t>;
extern template class series_registry<mpfr::mpreal, size_t>;

extern template class series_registry<std::complex<float>,       size_t>;
extern template class series_registry<std::complex<double>,      size_t>;
extern template class series_registry<std::complex<long double>, size_t>;
extern template class series_registry<std::complex<mpfr::mpreal>, size_t>;

extern template class series_registry<intprec::interval<float>,       size_t>;
extern template class series_registry<intprec::interval<double>,      size_t>;
extern template class series_registry<intprec::interval<long double>, size_t>;
extern template class series_registry<intprec::interval<mpfr::mpreal>, size_t>;

}} // namespace shanks::series
#endif // SHANKS_FFI_SHARED_BUILD

#endif
