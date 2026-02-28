#ifndef BRIDGE_INTERNAL_HPP
#define BRIDGE_INTERNAL_HPP
#pragma once

#include "bridge.hpp"
#include "ffi_internal.hpp"
#include "series.hpp"
#include "series_base.hpp"
#include "series_acceleration.hpp"
#include "methods.hpp"
#include "utils.hpp"
#include "noise/noise_generator.hpp"
#include "filters/kolmogorov_zurbenko.hpp"
#include "filters/savitzky_golay.hpp"

#include <complex>
#include <vector>
#include <memory>
#include <stdexcept>

#include "shanks-unified/src/ffi/bridge.rs.h"

namespace shanks::ffi::bridge {

inline std::string get_json_val(const std::string& json, const std::string& key) {
    auto pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";
    pos = json.find(":", pos);
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\"')) pos++;
    auto end = pos;
    while (end < json.size() && json[end] != ',' && json[end] != '}' && json[end] != '\"') end++;
    return json.substr(pos, end - pos);
}

// Conversion helpers
template <typename T>
inline RealValue to_rv(const T& v) {
    if constexpr (shanks::ffi::is_interval_v<T>) {
        auto sv = shanks::ffi::to_scientific(v.inf()); // Use inf as a fallback
        return RealValue{sv.mantissa, sv.exponent};
    } else {
        auto sv = shanks::ffi::to_scientific(v);
        return RealValue{sv.mantissa, sv.exponent};
    }
}

template <typename T>
inline ComplexValue to_cv(const T& v) {
    if constexpr (shanks::ffi::is_complex_v<T>) {
        return ComplexValue{to_rv(v.real()), to_rv(v.imag())};
    } else {
        return ComplexValue{to_rv(v), to_rv(0.0)};
    }
}

template <typename T>
inline IntervalValue to_iv(const T& v) {
    if constexpr (shanks::ffi::is_interval_v<T>) {
        return IntervalValue{to_rv(v.inf()), to_rv(v.sup())};
    } else {
        return IntervalValue{to_rv(v), to_rv(v)};
    }
}

template <typename T>
inline CIntervalValue to_civ(const T& v) {
    if constexpr (shanks::ffi::is_complex_interval_v<T>) {
        auto r = v.real();
        auto i = v.imag();
        return CIntervalValue{
            ComplexValue{to_rv(r.inf()), to_rv(i.inf())},
            ComplexValue{to_rv(r.sup()), to_rv(i.sup())}
        };
    } else if constexpr (shanks::ffi::is_complex_v<T>) {
        return CIntervalValue{to_cv(v.real()), to_cv(v.imag())};
    } else if constexpr (shanks::ffi::is_interval_v<T>) {
        return CIntervalValue{ComplexValue{to_rv(v.inf()), to_rv(0.0)}, ComplexValue{to_rv(v.sup()), to_rv(0.0)}};
    } else {
        return CIntervalValue{to_cv(v), to_cv(v)};
    }
}

template <typename T, PrecisionType P>
class CSeriesImpl : public CSeries {
    ::series_result<T> result;
    T limit;

    RawArr convert_vec(const std::vector<T>& vec) const {
        RawArr res;
        auto size = vec.size();
        if constexpr (is_complex_interval_v<T>) {
            res.tag = ArrKind::CInterval;
            res.r1.reserve(size); res.r2.reserve(size); res.r3.reserve(size); res.r4.reserve(size);
            for (const auto& v : vec) {
                auto civ = to_civ(v);
                res.r1.push_back(civ.inf.real); res.r2.push_back(civ.sup.real);
                res.r3.push_back(civ.inf.imag); res.r4.push_back(civ.sup.imag);
            }
        } else if constexpr (is_complex_v<T>) {
            res.tag = ArrKind::Complex;
            res.r1.reserve(size); res.r2.reserve(size);
            for (const auto& v : vec) {
                auto cv = to_cv(v);
                res.r1.push_back(cv.real); res.r2.push_back(cv.imag);
            }
        } else if constexpr (is_interval_v<T>) {
            res.tag = ArrKind::Interval;
            res.r1.reserve(size); res.r2.reserve(size);
            for (const auto& v : vec) {
                auto iv = to_iv(v);
                res.r1.push_back(iv.inf); res.r2.push_back(iv.sup);
            }
        } else {
            res.tag = ArrKind::Real;
            res.r1.reserve(size);
            for (const auto& v : vec) res.r1.push_back(to_rv(v));
        }
        return res;
    }

    RawValue convert_val(const T& val) const {
        if constexpr (is_complex_interval_v<T>) {
            auto civ = to_civ(val);
            RawValue res; res.tag = ValueKind::CInterval;
            res.r1 = civ.inf.real; res.r2 = civ.sup.real;
            res.r3 = civ.inf.imag; res.r4 = civ.sup.imag;
            return res;
        } else if constexpr (is_complex_v<T>) {
            auto cv = to_cv(val);
            RawValue res; res.tag = ValueKind::Complex;
            res.r1 = cv.real; res.r2 = cv.imag;
            return res;
        } else if constexpr (is_interval_v<T>) {
            auto iv = to_iv(val);
            RawValue res; res.tag = ValueKind::Interval;
            res.r1 = iv.inf; res.r2 = iv.sup;
            return res;
        } else {
            auto rv = to_rv(val);
            RawValue res; res.tag = ValueKind::Real;
            res.r1 = rv;
            return res;
        }
    }

public:
    CSeriesImpl(series_result<T> r, T s) : result(r), limit(s) {}

    PrecisionType precision_type() const override { return P; }
    const void* raw_result() const override { return &result; }

    RawArr get_sn() const override { return convert_vec(result.Sn); }
    RawArr get_an() const override { return convert_vec(result.an); }

    RawArr get_deviation() const override {
        try {
            RawArr res;
            auto size = result.Sn.size();

            if constexpr (is_complex_interval_v<T>) {
                res.tag = ArrKind::CInterval;
                res.r1.reserve(size); res.r2.reserve(size); res.r3.reserve(size); res.r4.reserve(size);
                for (const auto& s : result.Sn) {
                    res.r1.push_back(to_rv(s.real().inf() - limit.real().inf()));
                    res.r2.push_back(to_rv(s.real().sup() - limit.real().sup()));
                    res.r3.push_back(to_rv(s.imag().inf() - limit.imag().inf()));
                    res.r4.push_back(to_rv(s.imag().sup() - limit.imag().sup()));
                }
            } else if constexpr (is_complex_v<T>) {
                res.tag = ArrKind::Complex;
                res.r1.reserve(size); res.r2.reserve(size);
                for (const auto& s : result.Sn) {
                    res.r1.push_back(to_rv(s.real() - limit.real()));
                    res.r2.push_back(to_rv(s.imag() - limit.imag()));
                }
            } else if constexpr (is_interval_v<T>) {
                res.tag = ArrKind::Interval;
                res.r1.reserve(size); res.r2.reserve(size);
                for (const auto& s : result.Sn) {
                    res.r1.push_back(to_rv(s.inf() - limit.inf()));
                    res.r2.push_back(to_rv(s.sup() - limit.sup()));
                }
            } else {
                res.tag = ArrKind::Real;
                res.r1.reserve(size);
                for (const auto& s : result.Sn) {
                    res.r1.push_back(to_rv(s - limit));
                }
            }
            return res;
        } catch (...) {
            RawArr res; res.tag = ArrKind::Real; return res;
        }
    }

    RawValue get_limit() const override {
        return convert_val(limit);
    }

    std::unique_ptr<CSeries> apply_noise(rust::Str name, rust::Str params_json, uint64_t start_n) const override {
        std::string s_params(params_json);
        std::string s_name(name);
        NoiseType nt = NoiseType::uniform;
        if (s_name == "gaussian" || s_name == "Gaussian") nt = NoiseType::normal;
        else if (s_name == "poisson" || s_name == "Poisson") nt = NoiseType::poisson;

        NoiseMethod nm = NoiseMethod::jitter;
        std::string s_method = get_json_val(s_params, "method");
        if (s_method == "scaling" || s_method == "Scaling" || s_method == "Multiplicative") nm = NoiseMethod::scaling;

        unsigned long long seed = 0;
        try { seed = std::stoull(get_json_val(s_params, "seed")); } catch(...) { seed = pseudo_random_seed; }

        double p1 = 0, p2 = 0;
        try { p1 = std::stod(get_json_val(s_params, "param1")); } catch(...) {}
        try { p2 = std::stod(get_json_val(s_params, "param2")); } catch(...) {}

        ::series_result<T> tail_res = result;
        if (start_n > 0 && start_n < result.Sn.size()) {
            tail_res.Sn.erase(tail_res.Sn.begin(), tail_res.Sn.begin() + start_n);
            if (tail_res.an.size() >= start_n) {
                tail_res.an.erase(tail_res.an.begin(), tail_res.an.begin() + start_n);
            }
        }

        auto new_res = ::shanks::apply_noise<T, double>(tail_res, nm, nt, seed, p1, p2);
        return std::make_unique<CSeriesImpl<T, P>>(std::move(new_res), limit);
    }

    std::unique_ptr<CSeries> run_algo(rust::Str name, rust::Str params_json, size_t m, size_t n) const override {
        std::string s_name(name);
        std::string s_params(params_json);

        auto keys = shanks::algos::transformation_registry_metadata::get_keys();
        size_t idx = 0; bool found = false;
        for (; idx < keys.size(); ++idx) if (keys[idx] == s_name) { found = true; break; }
        if (!found) {
            auto names = shanks::algos::transformation_registry_metadata::get_names();
            for (idx = 0; idx < names.size(); ++idx) if (names[idx] == s_name) { found = true; break; }
        }
        if (!found) throw std::runtime_error("Algorithm not found: " + s_name);

        auto algo = shanks::algos::transformation_registry<T, size_t>::create_by_index(idx);

        ::series_result<T> acc_res;
        acc_res.Sn.reserve(n);
        acc_res.an.reserve(n);
        
        T prev_accel = T(0);
        for (size_t i = 1; i <= n; ++i) {
            // Compute acceleration for first i terms of the original series
            T accelerated = (*algo)(i, m, result);
            acc_res.Sn.push_back(accelerated);
            acc_res.an.push_back(accelerated - prev_accel);
            prev_accel = accelerated;
        }

        return std::make_unique<CSeriesImpl<T, P>>(std::move(acc_res), prev_accel);
    }

    RawArr filter(rust::Str name, rust::Str params_json, uint64_t start_n) const override {
        std::string s_name(name);
        std::string s_params(params_json);
        std::vector<T> data = result.Sn;
        if (start_n > 0 && start_n < data.size()) {
            data.erase(data.begin(), data.begin() + start_n);
        }

        std::vector<T> filtered;
        if (s_name == "kz" || s_name == "Kolmogorov-Zurbenko") {
            size_t m = 5, k = 3;
            try { m = std::stoul(get_json_val(s_params, "window_length")); } catch(...) {}
            try { k = std::stoul(get_json_val(s_params, "degree")); } catch(...) {}
            filtered = shanks::filters::kolmogorov_zurbenko_filter<T>(data, m, k);
        } else if (s_name == "sg" || s_name == "Savitzky-Golay") {
            size_t wl = 5, po = 2, der = 0; double delta = 1.0;
            try { wl = std::stoul(get_json_val(s_params, "window_length")); } catch(...) {}
            try { po = std::stoul(get_json_val(s_params, "polyorder")); } catch(...) {}
            try { der = std::stoul(get_json_val(s_params, "derive")); } catch(...) {}
            try { delta = std::stod(get_json_val(s_params, "delta")); } catch(...) {}

            if constexpr (!is_interval_v<T> && !is_complex_interval_v<T>) {
                filtered = shanks::filters::savitzky_golay_filter<T>(data, wl, po, der, T(delta));
            } else { filtered = data; }
        } else { filtered = data; }

        return convert_vec(filtered);
    }
};

template <typename T>
T parse_x(const std::string& x) {
    std::istringstream iss(x);
    T value;
    iss >> value;
    return value;
}

template <typename T, PrecisionType P>
std::unique_ptr<CSeries> mk_typed_series(size_t idx, const std::string& params_json, size_t n, const std::string& x_str) {
    T x = parse_x<T>(x_str);
    auto series = shanks::series::series_registry<T, size_t>::create(idx, x);
    return std::make_unique<CSeriesImpl<T, P>>(series->generate(n), series->get_sum());
}

// Declarations of typed factory for split build
std::unique_ptr<CSeries> mk_series_f64(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);
std::unique_ptr<CSeries> mk_series_arb(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);

} // namespace shanks::ffi::bridge

#endif
