#include "bridge_internal.hpp"

namespace shanks::ffi::bridge {

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

    auto names = shanks::series::series_registry_metadata::get_names();
    size_t idx = 0; bool found = false;
    for (; idx < names.size(); ++idx) if (names[idx] == s_name) { found = true; break; }
    if (!found) throw std::runtime_error("Series not found: " + s_name);

    std::unique_ptr<CSeries> res;
    if (res = mk_series_f64(idx, pt, s_params, n, s_x)) return res;
    if (res = mk_series_arb(idx, pt, s_params, n, s_x)) return res;
    
    throw std::runtime_error("Precision not yet implemented in bridge: " + s_prec);
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
RawArr filter(const CSeries& series, rust::Str name, rust::Str params_json, uint64_t start_n) { return series.filter(name, params_json, start_n); }

rust::Vec<rust::String> list_series() {
    rust::Vec<rust::String> res;
    auto names = shanks::series::series_registry_metadata::get_names();
    for (const auto& n : names) res.push_back(n);
    return res;
}

rust::Vec<rust::String> list_accels() {
    rust::Vec<rust::String> res;
    auto names = shanks::algos::transformation_registry_metadata::get_names();
    for (const auto& n : names) res.push_back(n);
    return res;
}

rust::Vec<rust::String> list_precisions() {
    rust::Vec<rust::String> res;
    res.push_back("F64"); res.push_back("Arb256"); res.push_back("Arb512");
    res.push_back("CF64"); res.push_back("CArb256");
    res.push_back("IntervalF64"); res.push_back("CIntervalF64");
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

} // namespace shanks::ffi::bridge
