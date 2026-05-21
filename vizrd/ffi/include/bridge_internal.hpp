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
#include "utils/json.hpp"
#include "noise/noise_generator.hpp"
#include "filters/kolmogorov_zurbenko.hpp"
#include "filters/savitzky_golay.hpp"
#include "filters/filter_generator.hpp"

#include <complex>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "vizrd/src/ffi/bridge.rs.h"

namespace shanks::ffi::bridge {

// Conversion helpers
template <typename T>
inline RealValue to_rv(const T& v) {
    if constexpr (::shanks::ffi::is_interval_v<T>) {
        auto sv = ::shanks::ffi::to_scientific(v.inf()); // Use inf as a fallback
        return RealValue{sv.mantissa, sv.exponent};
    } else {
        auto sv = ::shanks::ffi::to_scientific(v);
        return RealValue{sv.mantissa, sv.exponent};
    }
}

struct ComplexValue {
    RealValue real;
    RealValue imag;
};
struct IntervalValue {
    RealValue inf;
    RealValue sup;
};
struct CIntervalValue {
    ComplexValue inf;
    ComplexValue sup;
};

template <typename T>
inline ComplexValue to_cv(const T& v) {
    if constexpr (::shanks::ffi::is_complex_v<T>) {
        return ComplexValue{to_rv(v.real()), to_rv(v.imag())};
    } else {
        return ComplexValue{to_rv(v), to_rv(0.0)};
    }
}

template <typename T>
inline IntervalValue to_iv(const T& v) {
    if constexpr (::shanks::ffi::is_interval_v<T>) {
        return IntervalValue{to_rv(v.inf()), to_rv(v.sup())};
    } else {
        return IntervalValue{to_rv(v), to_rv(v)};
    }
}

template <typename T>
inline CIntervalValue to_civ(const T& v) {
    if constexpr (::shanks::ffi::is_complex_interval_v<T>) {
        auto r = v.real();
        auto i = v.imag();
        return CIntervalValue{
            ComplexValue{to_rv(r.inf()), to_rv(i.inf())},
            ComplexValue{to_rv(r.sup()), to_rv(i.sup())}
        };
    } else if constexpr (::shanks::ffi::is_complex_v<T>) {
        // Point: [re, re] + [im, im]i
        return CIntervalValue{
            ComplexValue{to_rv(v.real()), to_rv(v.imag())},
            ComplexValue{to_rv(v.real()), to_rv(v.imag())}
        };
    } else if constexpr (::shanks::ffi::is_interval_v<T>) {
        // Real Interval: [inf, sup] + [0, 0]i
        return CIntervalValue{
            ComplexValue{to_rv(v.inf()), to_rv(0.0)},
            ComplexValue{to_rv(v.sup()), to_rv(0.0)}
        };
    } else {
        // Scalar Point: [v, v] + [0, 0]i
        return CIntervalValue{
            ComplexValue{to_rv(v), to_rv(0.0)},
            ComplexValue{to_rv(v), to_rv(0.0)}
        };
    }
}

template <typename T, PrecisionType P>
class CSeriesImpl : public CSeries {
    ::series_result<T> result;
    T limit;
    // Errors occurred during the last run_algo() call.
    mutable std::vector<std::pair<uint64_t, std::string>> errors_;

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
        std::vector<T> dev;
        dev.reserve(result.Sn.size());
        for (const auto& sn : result.Sn) {
            dev.push_back(sn - limit);
        }
        return convert_vec(dev);
    }

    RawValue get_limit() const override {
        return convert_val(limit);
    }

    rust::Vec<ErrorEvent> get_errors() const override {
        rust::Vec<ErrorEvent> out;
        for (const auto& [n, msg] : errors_) {
            out.push_back(ErrorEvent{n, rust::String(msg)});
        }
        return out;
    }

    std::unique_ptr<CSeries> apply_noise(rust::Str name, rust::Str params_json, uint64_t start_n) const override {
        auto new_res = ::shanks::apply_noise<T>(result, std::string(name), std::string(params_json), start_n);
        return std::make_unique<CSeriesImpl<T, P>>(std::move(new_res), limit);
    }

    std::unique_ptr<CSeries> run_algo(rust::Str name, rust::Str params_json, size_t m, size_t n) const override {
        std::string s_name(name);
        std::string s_params(params_json);

        // Locate algorithm by camel-key or display-name
        auto keys = ::shanks::algos::transformation_registry_metadata::get_keys();
        size_t idx = 0; bool found = false;
        for (; idx < keys.size(); ++idx) if (keys[idx] == s_name) { found = true; break; }
        if (!found) {
            auto names = ::shanks::algos::transformation_registry_metadata::get_names();
            for (idx = 0; idx < names.size(); ++idx) if (names[idx] == s_name) { found = true; break; }
        }
        if (!found) throw std::runtime_error("Algorithm not found: " + s_name);

        auto algo = ::shanks::algos::transformation_registry<T, size_t>::create_by_index(idx, s_params);

        ::series_result<T> acc_res;
        acc_res.Sn.reserve(n);
        acc_res.an.reserve(n);

        std::vector<std::pair<uint64_t, std::string>> new_errors;

        T prev_accel = T(0);
        for (size_t i = 1; i <= n; ++i) {
            T accelerated;
            try {
                accelerated = (*algo)(i, m, result);
            } catch (const std::exception& ex) {
                new_errors.emplace_back(
                    static_cast<uint64_t>(i),
                    std::string(ex.what())
                );
                accelerated = prev_accel;
            } catch (...) {
                new_errors.emplace_back(
                    static_cast<uint64_t>(i),
                    "Unknown exception in acceleration algorithm"
                );
                accelerated = prev_accel;
            }
            acc_res.Sn.push_back(accelerated);
            acc_res.an.push_back(accelerated - prev_accel);
            prev_accel = accelerated;
        }

        // Build the result object and store errors on it
        auto result_obj = std::make_unique<CSeriesImpl<T, P>>(std::move(acc_res), limit);
        result_obj->errors_ = std::move(new_errors);
        return result_obj;
    }

    Filtered filter(rust::Str name, rust::Str params_json, uint64_t start_n) const override {
        std::string s_name(name);
        std::string s_params(params_json);
        try {
            auto filtered = ::shanks::filters::apply_filter<T>(result.Sn, s_name, s_params, start_n);
            std::vector<T> dev;
            dev.reserve(filtered.size());
            for (const auto& sn : filtered) {
                dev.push_back(sn - limit);
            }
            return Filtered{std::move(convert_vec(filtered)), std::move(convert_vec(dev))};
        } catch (const std::exception& ex) {
            throw std::runtime_error(
                "Filter '" + s_name + "' failed with params " + s_params + ": " + ex.what()
            );
        } catch (...) {
            throw std::runtime_error(
                "Filter '" + s_name + "' failed with params " + s_params + ": unknown exception"
            );
        }
    }
};

// ---------------------------------------------------------------------------
// parse_value<T> — type-aware value parser
//
// Real types:       delegate to parse_istream<T>
// Complex<T>:       parse "(re,im)" or bare real number
// Interval<T>:       parse "[inf,sup]" or bare real number  (future)
// ComplexInterval<T>: parse full 4-field object               (future)
// ---------------------------------------------------------------------------

/// Trim leading/trailing whitespace from a string.
static inline std::string trim_str(const std::string& s) {
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

/// Parse a real (non-complex, non-interval) value. Delegates to parse_istream<T>.
template <typename T>
    requires(!::shanks::ffi::is_complex_v<T> && !::shanks::ffi::is_interval_v<T> && !::shanks::ffi::is_complex_interval_v<T>)
T parse_value(const std::string& s) {
    return ::shanks::utils_json::parse_istream<T>(s);
}

/// Parse a complex value from "(real,imag)" format, or a bare real number.
/// Works with any T that supports parse_value<T> (including mpfr::mpreal).
/// Excludes complex<interval<T>> which is handled separately below.
template <typename T>
    requires(::shanks::ffi::is_complex_v<T> && !::shanks::ffi::is_complex_interval_v<T>)
T parse_value(const std::string& s) {
    using V = typename T::value_type;
    std::string trimmed = trim_str(s);

    if (!trimmed.empty() && trimmed.front() == '(' && trimmed.back() == ')') {
        // Find comma separating real and imag
        size_t comma = trimmed.find(',');
        if (comma == std::string::npos) {
            throw std::runtime_error("parse_value<complex>: missing comma in '" + s + "'");
        }
        std::string re_str = trim_str(trimmed.substr(1, comma - 1));
        std::string im_str = trim_str(trimmed.substr(comma + 1, trimmed.size() - comma - 2));
        V re = parse_value<V>(re_str);
        V im = parse_value<V>(im_str);
        return T(re, im);
    }

    // Bare real number: imag = 0
    V re = parse_value<V>(trimmed);
    return T(re, V(0));
}

/// Parse an interval value from "[inf,sup]" format, or a bare real number.
template <typename T>
    requires(::shanks::ffi::is_interval_v<T> && !::shanks::ffi::is_complex_interval_v<T>)
T parse_value(const std::string& s) {
    using V = typename T::value_type;
    std::string trimmed = trim_str(s);

    if (!trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']') {
        size_t comma = trimmed.find(',');
        if (comma == std::string::npos) {
            throw std::runtime_error("parse_value<interval>: missing comma in '" + s + "'");
        }
        std::string inf_str = trim_str(trimmed.substr(1, comma - 1));
        std::string sup_str = trim_str(trimmed.substr(comma + 1, trimmed.size() - comma - 2));
        V inf = parse_value<V>(inf_str);
        V sup = parse_value<V>(sup_str);
        return T(inf, sup);
    }

    // Bare real number: inf = sup = value
    V v = parse_value<V>(trimmed);
    return T(v, v);
}

/// Parse a complex-interval value from "([ri,rs],[ii,is])" or "(re,im)" format.
/// Depends on parse_value<complex<V>> and parse_value<interval<V>> above.
template <typename T>
    requires(::shanks::ffi::is_complex_interval_v<T>)
T parse_value(const std::string& s) {
    using IV = typename T::value_type;  // intprec::interval<V>
    using V = typename IV::value_type;
    std::string trimmed = trim_str(s);

    if (!trimmed.empty() && trimmed.front() == '(' && trimmed.back() == ')') {
        // Try to find the comma separating real-interval from imag-interval.
        // This is tricky because intervals contain commas: ([a,b],[c,d])
        // Strategy: find matching brackets.
        auto find_top_comma = [](const std::string& t) -> size_t {
            int depth = 0;
            for (size_t i = 1; i + 1 < t.size(); ++i) {
                if (t[i] == '[') ++depth;
                else if (t[i] == ']') --depth;
                else if (t[i] == ',' && depth == 0) return i;
            }
            return std::string::npos;
        };

        size_t comma = find_top_comma(trimmed);
        if (comma == std::string::npos) {
            // Might be (re,im) with plain reals → degenerate intervals
            // Parse as complex<V> first, then wrap in intervals
            std::complex<V> cv = parse_value<std::complex<V>>(s);
            return T(IV(cv.real(), cv.real()), IV(cv.imag(), cv.imag()));
        }

        std::string re_str = trim_str(trimmed.substr(1, comma - 1));
        std::string im_str = trim_str(trimmed.substr(comma + 1, trimmed.size() - comma - 2));
        IV re_iv = parse_value<IV>(re_str);
        IV im_iv = parse_value<IV>(im_str);
        return T(re_iv, im_iv);
    }

    // Bare real number: degenerate interval
    V v = parse_value<V>(trimmed);
    return T(IV(v, v), IV(V(0), V(0)));
}

template <typename T, PrecisionType P>
std::unique_ptr<CSeries> mk_typed_series(size_t idx, const std::string& params_json, size_t n, const std::string& x_str) {
    T x = parse_value<T>(x_str);

    // Parse optional T-parameter (alpha) and K-parameter (m) from params_json.
    // These map to the addTParameter / addKParameter accepted by series_registry::create,
    // which then dispatches bin_iterator(x, alpha), incomplete_Gamma_func_iterator(x, alpha),
    // m_fact_1mx_mp1_inverse_iterator(x, k), etc. via if-constexpr in the factory lambda.
    const auto& entry = ::shanks::series::series_registry<T, size_t>::get_entries()[idx];
    T t_param = ::utils::cast<T, int>()(1);
    size_t k_param = 1;

    if (entry.needsT) {
        t_param = ::utils::cast<T, double>()(::shanks::utils_json::parse_double(::shanks::utils_json::get_json_val_required(params_json, "alpha")));
    }
    if (entry.needsK) {
        k_param = ::shanks::utils_json::parse_ulong(::shanks::utils_json::get_json_val_required(params_json, "m"));
    }

    auto series = ::shanks::series::series_registry<T, size_t>::create(idx, x, t_param, k_param);
    return std::make_unique<CSeriesImpl<T, P>>(series->generate(n), series->get_sum());
}


// Declarations of typed factory for split build
std::unique_ptr<CSeries> mk_series_f32(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);
std::unique_ptr<CSeries> mk_series_f64(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);
std::unique_ptr<CSeries> mk_series_flong(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);
std::unique_ptr<CSeries> mk_series_arb(size_t idx, PrecisionType pt, const std::string& params_json, size_t n, const std::string& x);

// Convert a raw double to type T, handling interval/complex wrappers.
template <typename T> struct make_value_impl { static T convert(double v); };

#define SHANKS_MAKE_VALUE_SPEC(TYPE, EXPR) \
    template <> struct make_value_impl<TYPE> { static TYPE convert(double v) { return EXPR; } };

SHANKS_MAKE_VALUE_SPEC(float, static_cast<float>(v))
SHANKS_MAKE_VALUE_SPEC(double, v)
SHANKS_MAKE_VALUE_SPEC(long double, static_cast<long double>(v))
SHANKS_MAKE_VALUE_SPEC(mpfr::mpreal, mpfr::mpreal(v))

#undef SHANKS_MAKE_VALUE_SPEC

template <typename T> struct make_value_impl<std::complex<T>> {
    static std::complex<T> convert(double v) { return std::complex<T>(make_value_impl<T>::convert(v)); }
};

template <typename T> struct make_value_impl<intprec::interval<T>> {
    static intprec::interval<T> convert(double v) { return intprec::interval<T>(make_value_impl<T>::convert(v)); }
};

template <typename T> struct make_value_impl<std::complex<intprec::interval<T>>> {
    static std::complex<intprec::interval<T>> convert(double v) {
        return std::complex<intprec::interval<T>>(make_value_impl<intprec::interval<T>>::convert(v));
    }
};

template <typename T> inline T make_value(double v) { return make_value_impl<T>::convert(v); }

template <typename T, PrecisionType P>
std::unique_ptr<CSeries> mk_series_from_sn_typed(const std::vector<double>& raw_sn) {
    std::vector<T> sn;
    sn.reserve(raw_sn.size());
    for (double v : raw_sn) sn.push_back(make_value<T>(v));

    // Compute an: an[0] = sn[0], an[i] = sn[i] - sn[i-1]
    std::vector<T> an(sn.size());
    if (!sn.empty()) {
        an[0] = sn[0];
        for (size_t i = 1; i < sn.size(); ++i) {
            an[i] = sn[i] - sn[i - 1];
        }
    }

    T limit = sn.empty() ? make_value<T>(0) : sn.back(); // Last Sn value as limit estimate

    series_result<T> result;
    result.Sn = std::move(sn);
    result.an = std::move(an);

    return std::make_unique<CSeriesImpl<T, P>>(std::move(result), limit);
}

} // namespace shanks::ffi::bridge

#endif
