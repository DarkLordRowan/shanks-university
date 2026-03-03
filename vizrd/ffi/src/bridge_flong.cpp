#include "bridge_internal.hpp"

namespace shanks::ffi::bridge {

std::unique_ptr<CSeries> mk_series_flong(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x) {
    switch (pt) {
        case PrecisionType::FLong: return mk_typed_series<long double, PrecisionType::FLong>(idx, params_json, n, x);
        case PrecisionType::CFLong: return mk_typed_series<std::complex<long double>, PrecisionType::CFLong>(idx, params_json, n, x);
        case PrecisionType::IntervalFLong: return mk_typed_series<intprec::interval<long double>, PrecisionType::IntervalFLong>(idx, params_json, n, x);
        case PrecisionType::CIntervalFLong: return mk_typed_series<std::complex<intprec::interval<long double>>, PrecisionType::CIntervalFLong>(idx, params_json, n, x);
        default: return nullptr;
    }
}

} // namespace shanks::ffi::bridge
