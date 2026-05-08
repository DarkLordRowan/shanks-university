#include "bridge_internal.hpp"
#include <gsl/gsl_errno.h>

namespace shanks::ffi::bridge {

// Turn off GSL's default error handler (which calls abort()) globally
struct GslErrorHandlerOff {
    GslErrorHandlerOff() {
        gsl_set_error_handler_off();
    }
} gsl_error_handler_off_instance;

void shanks_force_link_gslcblas() {
    // dummy call to ensure linkage
}

std::unique_ptr<CSeries> mk_series(rust::Str name, rust::Str precision, rust::Str params_json, size_t n, rust::Str x) {
    std::string s_name = std::string(name);
    std::string s_prec = std::string(precision);
    std::string s_params = std::string(params_json);
    std::string s_x = std::string(x);
    PrecisionType pt;
    if (!parse_precision(s_prec.c_str(), pt)) throw std::runtime_error("Invalid precision: " + s_prec);

    auto names = ::shanks::series::series_registry_metadata::get_names();
    size_t idx = 0; bool found = false;
    for (; idx < names.size(); ++idx) if (names[idx] == s_name) { found = true; break; }
    if (!found) throw std::runtime_error("Series not found: " + s_name);

    std::unique_ptr<CSeries> res;
    if ((res = mk_series_f32(idx, pt, s_params, n, s_x))) return res;
    if ((res = mk_series_f64(idx, pt, s_params, n, s_x))) return res;
    if ((res = mk_series_flong(idx, pt, s_params, n, s_x))) return res;
    if ((res = mk_series_arb(idx, pt, s_params, n, s_x))) return res;

    throw std::runtime_error("Precision not yet implemented in bridge: " + s_prec);
}

std::unique_ptr<CSeries> mk_series_from_sn(rust::Str precision, rust::Vec<rust::String> sn, size_t n) {
    std::string s_prec(precision);
    PrecisionType pt;
    if (!parse_precision(s_prec.c_str(), pt)) throw std::runtime_error("Invalid precision: " + s_prec);

    // Parse number-strings to doubles, capped at n
    size_t count = std::min(n, sn.size());
    std::vector<double> raw_sn;
    raw_sn.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        try {
            raw_sn.push_back(std::stod(std::string(sn[i])));
        } catch (...) {
            throw std::runtime_error("mk_series_from_sn: failed to parse '" + std::string(sn[i]) + "' as number");
        }
    }

    switch (pt) {
        case PrecisionType::F32:
            return mk_series_from_sn_typed<float, PrecisionType::F32>(raw_sn);
        case PrecisionType::CF32:
            return mk_series_from_sn_typed<std::complex<float>, PrecisionType::CF32>(raw_sn);
        case PrecisionType::IntervalF32:
            return mk_series_from_sn_typed<intprec::interval<float>, PrecisionType::IntervalF32>(raw_sn);
        case PrecisionType::CIntervalF32:
            return mk_series_from_sn_typed<std::complex<intprec::interval<float>>, PrecisionType::CIntervalF32>(raw_sn);

        case PrecisionType::F64:
            return mk_series_from_sn_typed<double, PrecisionType::F64>(raw_sn);
        case PrecisionType::CF64:
            return mk_series_from_sn_typed<std::complex<double>, PrecisionType::CF64>(raw_sn);
        case PrecisionType::IntervalF64:
            return mk_series_from_sn_typed<intprec::interval<double>, PrecisionType::IntervalF64>(raw_sn);
        case PrecisionType::CIntervalF64:
            return mk_series_from_sn_typed<std::complex<intprec::interval<double>>, PrecisionType::CIntervalF64>(raw_sn);

        case PrecisionType::FLong:
            return mk_series_from_sn_typed<long double, PrecisionType::FLong>(raw_sn);
        case PrecisionType::CFLong:
            return mk_series_from_sn_typed<std::complex<long double>, PrecisionType::CFLong>(raw_sn);
        case PrecisionType::IntervalFLong:
            return mk_series_from_sn_typed<intprec::interval<long double>, PrecisionType::IntervalFLong>(raw_sn);
        case PrecisionType::CIntervalFLong:
            return mk_series_from_sn_typed<std::complex<intprec::interval<long double>>, PrecisionType::CIntervalFLong>(raw_sn);

        case PrecisionType::Arb:
            return mk_series_from_sn_typed<mpfr::mpreal, PrecisionType::Arb>(raw_sn);
        case PrecisionType::CArb:
            return mk_series_from_sn_typed<std::complex<mpfr::mpreal>, PrecisionType::CArb>(raw_sn);
        case PrecisionType::IntervalArb:
            return mk_series_from_sn_typed<intprec::interval<mpfr::mpreal>, PrecisionType::IntervalArb>(raw_sn);
        case PrecisionType::CIntervalArb:
            return mk_series_from_sn_typed<std::complex<intprec::interval<mpfr::mpreal>>, PrecisionType::CIntervalArb>(raw_sn);
    }
}

std::unique_ptr<CSeries> apply_noise(const CSeries& series, rust::Str name, rust::Str params_json, uint64_t start_n) {
    return series.apply_noise(name, params_json, start_n);
}

std::unique_ptr<CSeries> run_algo(const CSeries& series, rust::Str name, rust::Str params_json, size_t m, size_t n) {
    return series.run_algo(name, params_json, m, n);
}

RawArr get_sn(const CSeries& series) { return series.get_sn(); }
RawArr get_an(const CSeries& series) { return series.get_an(); }
RawArr get_deviation(const CSeries& series) { return series.get_deviation(); }
RawValue get_limit(const CSeries& series) { return series.get_limit(); }
Filtered filter(const CSeries& series, rust::Str name, rust::Str params_json, uint64_t start_n) { return series.filter(name, params_json, start_n); }
rust::Vec<ErrorEvent> get_errors(const CSeries& series) { return series.get_errors(); }


rust::Vec<rust::String> list_series() {
    rust::Vec<rust::String> res;
    auto names = ::shanks::series::series_registry_metadata::get_names();
    for (const auto& n : names) res.push_back(n);
    return res;
}

rust::Vec<rust::String> list_accels() {
    rust::Vec<rust::String> res;
    auto names = ::shanks::algos::transformation_registry_metadata::get_names();
    for (const auto& n : names) res.push_back(n);
    return res;
}

rust::Vec<rust::String> list_precisions() {
    rust::Vec<rust::String> res;
    res.push_back("F32"); res.push_back("F64"); res.push_back("FLong"); res.push_back("Arb");
    res.push_back("CF32"); res.push_back("CF64"); res.push_back("CFLong"); res.push_back("CArb");
    res.push_back("IntervalF32"); res.push_back("IntervalF64"); res.push_back("IntervalFLong"); res.push_back("IntervalArb");
    res.push_back("CIntervalF32"); res.push_back("CIntervalF64"); res.push_back("CIntervalFLong"); res.push_back("CIntervalArb");
    return res;
}

rust::Vec<rust::String> list_noises() {
    rust::Vec<rust::String> res;
    res.push_back("Uniform"); res.push_back("Gaussian"); res.push_back("Poisson");
    return res;
}

rust::Vec<rust::String> list_noise_methods() {
    rust::Vec<rust::String> res;
    res.push_back("Additive"); res.push_back("Multiplicative");
    return res;
}

// void set_default_precision(size_t prec) {
//     mpfr::mpreal::set_default_prec(static_cast<mp_prec_t>(prec));
// }

} // namespace shanks::ffi::bridge
