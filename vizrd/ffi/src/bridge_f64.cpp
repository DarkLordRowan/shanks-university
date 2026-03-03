#include "bridge_internal.hpp"

namespace shanks::ffi::bridge {

std::unique_ptr<CSeries> mk_series_f64(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x) {
    switch (pt) {
        case PrecisionType::F64: return mk_typed_series<double, PrecisionType::F64>(idx, params_json, n, x);
        case PrecisionType::CF64: return mk_typed_series<std::complex<double>, PrecisionType::CF64>(idx, params_json, n, x);
        case PrecisionType::IntervalF64: return mk_typed_series<intprec::interval<double>, PrecisionType::IntervalF64>(idx, params_json, n, x);
        case PrecisionType::CIntervalF64: return mk_typed_series<std::complex<intprec::interval<double>>, PrecisionType::CIntervalF64>(idx, params_json, n, x);
        default: return nullptr;
    }
}

} // namespace shanks::ffi::bridge
