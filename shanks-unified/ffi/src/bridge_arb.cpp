#include "bridge_internal.hpp"

namespace shanks::ffi::bridge {

std::unique_ptr<CSeries> mk_series_arb(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x) {
    switch (pt) {
        case PrecisionType::Arb: return mk_typed_series<mpfr::mpreal, PrecisionType::Arb>(idx, params_json, n, x);
        case PrecisionType::CArb: return mk_typed_series<std::complex<mpfr::mpreal>, PrecisionType::CArb>(idx, params_json, n, x);
        case PrecisionType::IntervalArb: return mk_typed_series<intprec::interval<mpfr::mpreal>, PrecisionType::IntervalArb>(idx, params_json, n, x);
        case PrecisionType::CIntervalArb: return mk_typed_series<std::complex<intprec::interval<mpfr::mpreal>>, PrecisionType::CIntervalArb>(idx, params_json, n, x);
        default: return nullptr;
    }
}

} // namespace shanks::ffi::bridge
