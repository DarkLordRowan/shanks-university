/**
 * @file series_registry_impl.cpp
 * @brief Explicit template instantiations for series_registry<T, K>.
 *
 * On MinGW (Windows), static local variables inside function templates (e.g.,
 * the `entries` static local in `series_registry<T,K>::get_entries()`) are not
 * reliably deduplicated across translation units when building a shared DLL.
 * The linker reports them as undefined references.
 *
 * The fix: suppress implicit instantiation in all other TUs via
 * `extern template class series_registry<T,K>` (added to series.hpp under the
 * SHANKS_FFI_SHARED_BUILD guard), and provide the one canonical explicit
 * instantiation definition here.
 *
 * We undef SHANKS_FFI_SHARED_BUILD before including lib.hpp so that series.hpp
 * does NOT emit `extern template` in this TU — we need the full definition
 * here for the explicit instantiation definitions that follow.
 */

#ifdef SHANKS_FFI_SHARED_BUILD
#undef SHANKS_FFI_SHARED_BUILD
#endif

#include "../../core/include/lib.hpp"

// Provide the explicit instantiation definition for every specialisation used
// by the FFI layer.  These must come AFTER the full template definition is
// visible (i.e., after lib.hpp), and must NOT be prefixed with `extern`.

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

template class series_registry<std::complex<intprec::interval<float>>,       size_t>;
template class series_registry<std::complex<intprec::interval<double>>,      size_t>;
template class series_registry<std::complex<intprec::interval<long double>>, size_t>;

}} // namespace shanks::series
