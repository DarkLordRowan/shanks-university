#include "bridge_internal.hpp"

namespace shanks::ffi::bridge {

std::unique_ptr<CSeries> mk_series_f32(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x) {
    switch (pt) {
        case PrecisionType::F32: return mk_typed_series<float, PrecisionType::F32>(idx, params_json, n, x);
        case PrecisionType::CF32: return mk_typed_series<std::complex<float>, PrecisionType::CF32>(idx, params_json, n, x);
        case PrecisionType::IntervalF32: return mk_typed_series<intprec::interval<float>, PrecisionType::IntervalF32>(idx, params_json, n, x);
        case PrecisionType::CIntervalF32: return mk_typed_series<std::complex<intprec::interval<float>>, PrecisionType::CIntervalF32>(idx, params_json, n, x);
        default: return nullptr;
    }
}

} // namespace shanks::ffi::bridge
