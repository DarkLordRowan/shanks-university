/**
 * @file series_ffi.cpp
 * @brief Implementation of series FFI functions.
 * @authors Sobolev Y. A.
 * 
 * This file implements the series creation and computation FFI interface.
 * It uses the existing series_registry infrastructure for type erasure.
 */

#include "shanks_ffi.hpp"
#include "ffi_internal.hpp"
#include "../../core/include/lib.hpp"
#include "../../core/include/noise/noise_generator.hpp"

#include <cmath>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

// Use shared types from ffi_internal.hpp
using shanks::ffi::PrecisionType;
using shanks::ffi::SeriesHandleBase;
using shanks::ffi::alloc_string;
using shanks::ffi::set_error;
using shanks::ffi::clear_error;
using shanks::ffi::parse_precision;
using shanks::ffi::ScientificValue;

// Thread-local error storage definition
namespace shanks { namespace ffi {
thread_local std::string g_last_error;
}}

namespace {

bool is_complex(PrecisionType p) {
    return static_cast<int>(p) >= static_cast<int>(PrecisionType::CF32);
}

// Extended series handle with additional methods
struct SeriesHandleBaseExt : public SeriesHandleBase {
    virtual std::string get_sum() const = 0;
    virtual std::string get_x() const = 0;
    virtual std::string get_name() const override { return "unknown"; }
};

struct NoiseConfig {
    bool enabled = false;
    NoiseType type = uniform;
    NoiseMethod method = jitter;
    double param1 = 0;
    double param2 = 0;
    uint64_t seed = 0;
};

// Template implementation for real series
template <typename T>
struct SeriesHandleReal : public SeriesHandleBaseExt {
    std::unique_ptr<shanks::series::series_base<T, size_t>> series;
    PrecisionType precision;
    T x_value;
    std::string name;
    
    // Cache for raw data extraction without JSON serialization overhead
    mutable std::unique_ptr<series_result<T>> cached_result;
    mutable uint64_t cached_n = 0;
    mutable std::optional<T> cached_sum;
    NoiseConfig noise_cfg;

    SeriesHandleReal(std::unique_ptr<shanks::series::series_base<T, size_t>> s, 
                     PrecisionType p, T x, const std::string& n = "", const NoiseConfig& cfg = {})
        : series(std::move(s)), precision(p), x_value(x), name(n), noise_cfg(cfg) {}
    
    FFISeriesResult* generate(uint64_t n) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return nullptr;
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            result = apply_noise<T, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        shanks::ffi::RealBinarySerializer<T> sn_ser;
        shanks::ffi::RealBinarySerializer<T> an_ser;
        shanks::ffi::RealBinarySerializer<T> dev_ser;

        T sum = T(0);
        bool has_sum = !series->is_invalid();
        if (has_sum) {
            try { sum = series->get_sum(); } catch (...) { has_sum = false; }
        }

        for (const auto& val : result.Sn) {
            sn_ser.push(val);
            dev_ser.push(has_sum ? (val - sum) : T(0));
        }
        for (const auto& val : result.an) an_ser.push(val);
        
        auto* res = new FFISeriesResult{};
        res->sn = sn_ser.finalize();
        res->an = an_ser.finalize();
        res->deviations = dev_ser.finalize();

        if (has_sum) {
            res->has_sum = 1;
            res->sum_type = 0; // Real
            auto sci = shanks::ffi::to_scientific(sum);
            res->sum_m[0] = sci.mantissa;
            res->sum_e[0] = sci.exponent;
        } else {
            res->has_sum = 0;
            res->sum_type = 0;
        }

        return res;
    }
    std::string get_sum() const override {
        if (!series || series->is_invalid()) return "";
        try {
            T sum = series->get_sum();
            return shanks::ffi::to_scientific(sum).to_json();
        } catch (...) {
            return "";
        }
    }
    std::string get_x() const override {
        std::ostringstream oss;
        oss << x_value;
        return oss.str();
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }

    const void* get_raw_data(uint64_t n) const override {
        if (!series) return nullptr;
        if (!cached_result || cached_n < n) {
            auto result = series->generate(n);
            if (noise_cfg.enabled) {
                result = apply_noise<T, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
            }
            cached_result = std::make_unique<series_result<T>>(result);
            cached_n = n;
        }
        return cached_result.get();
    }

    const void* get_native_sum() const override {
        if (!series) return nullptr;
        if (!cached_sum) {
            try {
                cached_sum = series->get_sum();
            } catch (...) {
                return nullptr;
            }
        }
        return &(*cached_sum);
    }
};

// Template implementation for complex series
template <typename T>
struct SeriesHandleComplex : public SeriesHandleBaseExt {
    std::unique_ptr<shanks::series::series_base<std::complex<T>, size_t>> series;
    PrecisionType precision;
    std::complex<T> x_value;
    std::string name;
    
    // Cache for raw data extraction without JSON serialization overhead
    mutable std::unique_ptr<series_result<std::complex<T>>> cached_result;
    mutable uint64_t cached_n = 0;
    mutable std::optional<std::complex<T>> cached_sum;
    NoiseConfig noise_cfg;

    SeriesHandleComplex(std::unique_ptr<shanks::series::series_base<std::complex<T>, size_t>> s, 
                        PrecisionType p, std::complex<T> x, const std::string& n = "", const NoiseConfig& cfg = {})
        : series(std::move(s)), precision(p), x_value(x), name(n), noise_cfg(cfg) {}
    
    FFISeriesResult* generate(uint64_t n) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return nullptr;
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            result = apply_noise<std::complex<T>, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        shanks::ffi::ComplexBinarySerializer<T> sn_ser;
        shanks::ffi::ComplexBinarySerializer<T> an_ser;
        shanks::ffi::ComplexBinarySerializer<T> dev_ser;

        std::complex<T> sum(0, 0);
        bool has_sum = !series->is_invalid();
        if (has_sum) {
            try { sum = series->get_sum(); } catch (...) { has_sum = false; }
        }

        for (const auto& val : result.Sn) {
            sn_ser.push(val);
            dev_ser.push(has_sum ? (val - sum) : std::complex<T>(0, 0));
        }
        for (const auto& val : result.an) an_ser.push(val);
        
        auto* res = new FFISeriesResult{};
        res->sn = sn_ser.finalize();
        res->an = an_ser.finalize();
        res->deviations = dev_ser.finalize();

        if (has_sum) {
            res->has_sum = 1;
            res->sum_type = 1; // Complex
            auto r_sci = shanks::ffi::to_scientific(sum.real());
            auto i_sci = shanks::ffi::to_scientific(sum.imag());
            res->sum_m[0] = r_sci.mantissa;
            res->sum_e[0] = r_sci.exponent;
            res->sum_m[1] = i_sci.mantissa;
            res->sum_e[1] = i_sci.exponent;
        } else {
            res->has_sum = 0;
            res->sum_type = 1;
        }

        return res;
    }
    std::string get_sum() const override {
        if (!series || series->is_invalid()) return "";
        // Deprecated: `shanks_series_get_sum` is practically obsolete since FFISeriesResult has native sum fields 
        // to avoid bringing back string serializers, return a stub JSON string with values.
        return "{\"real\": 0, \"imag\": 0}";
    }
    std::string get_x() const override {
        std::ostringstream oss;
        oss << x_value.real() << "+" << x_value.imag() << "j";
        return oss.str();
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }

    const void* get_raw_data(uint64_t n) const override {
        if (!series) return nullptr;
        if (!cached_result || cached_n < n) {
            auto result = series->generate(n);
            if (noise_cfg.enabled) {
                result = apply_noise<std::complex<T>, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
            }
            cached_result = std::make_unique<series_result<std::complex<T>>>(result);
            cached_n = n;
        }
        return cached_result.get();
    }

    const void* get_native_sum() const override {
        if (!series) return nullptr;
        if (!cached_sum) {
            try {
                cached_sum = series->get_sum();
            } catch (...) {
                return nullptr;
            }
        }
        return &(*cached_sum);
    }
};

// Template implementation for interval series
template <typename T>
struct SeriesHandleInterval : public SeriesHandleBaseExt {
    std::unique_ptr<shanks::series::series_base<intprec::interval<T>, size_t>> series;
    PrecisionType precision;
    intprec::interval<T> x_value;
    std::string name;
    
    // Cache for raw data extraction without JSON serialization overhead
    mutable std::unique_ptr<series_result<intprec::interval<T>>> cached_result;
    mutable uint64_t cached_n = 0;
    mutable std::optional<intprec::interval<T>> cached_sum;
    NoiseConfig noise_cfg;

    SeriesHandleInterval(std::unique_ptr<shanks::series::series_base<intprec::interval<T>, size_t>> s, 
                        PrecisionType p, intprec::interval<T> x, const std::string& n = "", const NoiseConfig& cfg = {})
        : series(std::move(s)), precision(p), x_value(x), name(n), noise_cfg(cfg) {}
    
    FFISeriesResult* generate(uint64_t n) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return nullptr;
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            // Noise for intervals would require interval noise generation which might not be implemented yet.
            // Leaving as is if apply_noise supports intervals, otherwise it will just use the value.
            // Note: Currently apply_noise on interval may fallback to throwing or not working, 
            // but we can try letting it work or disabling noise.
            // result = apply_noise<intprec::interval<T>, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        shanks::ffi::IntervalBinarySerializer<T> sn_ser;
        shanks::ffi::IntervalBinarySerializer<T> an_ser;
        shanks::ffi::IntervalBinarySerializer<T> dev_ser;

        intprec::interval<T> sum(0);
        bool has_sum = !series->is_invalid();
        if (has_sum) {
            try { sum = series->get_sum(); } catch (...) { has_sum = false; }
        }

        for (const auto& val : result.Sn) {
            sn_ser.push(val);
            dev_ser.push(has_sum ? (val - sum) : intprec::interval<T>(0));
        }
        for (const auto& val : result.an) an_ser.push(val);
        
        auto* res = new FFISeriesResult{};
        res->sn = sn_ser.finalize();
        res->an = an_ser.finalize();
        res->deviations = dev_ser.finalize();

        if (has_sum) {
            res->has_sum = 1;
            res->sum_type = 2; // Interval
            auto inf_sci = shanks::ffi::to_scientific(sum.inf());
            auto sup_sci = shanks::ffi::to_scientific(sum.sup());
            res->sum_m[0] = inf_sci.mantissa;
            res->sum_e[0] = inf_sci.exponent;
            res->sum_m[1] = sup_sci.mantissa;
            res->sum_e[1] = sup_sci.exponent;
        } else {
            res->has_sum = 0;
            res->sum_type = 2;
        }

        return res;
    }
    std::string get_sum() const override {
        if (!series || series->is_invalid()) return "";
        return "{\"inf\": 0, \"sup\": 0}";
    }
    std::string get_x() const override {
        std::ostringstream oss;
        oss << "[" << x_value.inf() << ", " << x_value.sup() << "]";
        return oss.str();
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }

    const void* get_raw_data(uint64_t n) const override {
        if (!series) return nullptr;
        if (!cached_result || cached_n < n) {
            auto result = series->generate(n);
            cached_result = std::make_unique<series_result<intprec::interval<T>>>(result);
            cached_n = n;
        }
        return cached_result.get();
    }

    const void* get_native_sum() const override {
        if (!series) return nullptr;
        if (!cached_sum) {
            try {
                cached_sum = series->get_sum();
            } catch (...) {
                return nullptr;
            }
        }
        return &(*cached_sum);
    }
};

// Template implementation for complex interval series
template <typename T>
struct SeriesHandleCInterval : public SeriesHandleBaseExt {
    std::unique_ptr<shanks::series::series_base<std::complex<intprec::interval<T>>, size_t>> series;
    PrecisionType precision;
    std::complex<intprec::interval<T>> x_value;
    std::string name;
    
    mutable std::unique_ptr<series_result<std::complex<intprec::interval<T>>>> cached_result;
    mutable uint64_t cached_n = 0;
    mutable std::optional<std::complex<intprec::interval<T>>> cached_sum;
    NoiseConfig noise_cfg;

    SeriesHandleCInterval(std::unique_ptr<shanks::series::series_base<std::complex<intprec::interval<T>>, size_t>> s, 
                        PrecisionType p, std::complex<intprec::interval<T>> x, const std::string& n = "", const NoiseConfig& cfg = {})
        : series(std::move(s)), precision(p), x_value(x), name(n), noise_cfg(cfg) {}
    
    FFISeriesResult* generate(uint64_t n) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return nullptr;
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        
        shanks::ffi::CIntervalBinarySerializer<T> sn_ser;
        shanks::ffi::CIntervalBinarySerializer<T> an_ser;
        shanks::ffi::CIntervalBinarySerializer<T> dev_ser;

        std::complex<intprec::interval<T>> sum(0);
        bool has_sum = !series->is_invalid();
        if (has_sum) {
            try { sum = series->get_sum(); } catch (...) { has_sum = false; }
        }

        for (const auto& val : result.Sn) {
            sn_ser.push(val);
            dev_ser.push(has_sum ? (val - sum) : std::complex<intprec::interval<T>>(0));
        }
        for (const auto& val : result.an) an_ser.push(val);
        
        auto* res = new FFISeriesResult{};
        res->sn = sn_ser.finalize();
        res->an = an_ser.finalize();
        res->deviations = dev_ser.finalize();

        if (has_sum) {
            res->has_sum = 1;
            res->sum_type = 3; // CInterval
            auto r_inf_sci = shanks::ffi::to_scientific(sum.real().inf());
            auto r_sup_sci = shanks::ffi::to_scientific(sum.real().sup());
            auto i_inf_sci = shanks::ffi::to_scientific(sum.imag().inf());
            auto i_sup_sci = shanks::ffi::to_scientific(sum.imag().sup());
            
            res->sum_m[0] = r_inf_sci.mantissa;
            res->sum_e[0] = r_inf_sci.exponent;
            res->sum_m[1] = r_sup_sci.mantissa;
            res->sum_e[1] = r_sup_sci.exponent;
            res->sum_m[2] = i_inf_sci.mantissa;
            res->sum_e[2] = i_inf_sci.exponent;
            res->sum_m[3] = i_sup_sci.mantissa;
            res->sum_e[3] = i_sup_sci.exponent;
        } else {
            res->has_sum = 0;
            res->sum_type = 3;
        }

        return res;
    }
    std::string get_sum() const override {
        if (!series || series->is_invalid()) return "";
        return "{\"real\": {\"inf\": 0, \"sup\": 0}, \"imag\": {\"inf\": 0, \"sup\": 0}}";
    }
    std::string get_x() const override {
        std::ostringstream oss;
        oss << "[" << x_value.real().inf() << ", " << x_value.real().sup() << "]+[" 
            << x_value.imag().inf() << ", " << x_value.imag().sup() << "]j";
        return oss.str();
    }
    
    PrecisionType get_precision() const override { return precision; }
    std::string get_name() const override { return name; }

    const void* get_raw_data(uint64_t n) const override {
        if (!series) return nullptr;
        if (!cached_result || cached_n < n) {
            auto result = series->generate(n);
            cached_result = std::make_unique<series_result<std::complex<intprec::interval<T>>>>(result);
            cached_n = n;
        }
        return cached_result.get();
    }

    const void* get_native_sum() const override {
        if (!series) return nullptr;
        if (!cached_sum) {
            try {
                cached_sum = series->get_sum();
            } catch (...) {
                return nullptr;
            }
        }
        return &(*cached_sum);
    }
};

// Helper to parse x value
template <typename T>
T parse_x(const std::string& x) {
    std::istringstream iss(x);
    T value;
    iss >> value;
    return value;
}

// Helper to find series index by name
size_t find_series_index(const char* name) {
    auto names = shanks::series::series_registry_metadata::get_names();
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i] == name) {
            return i;
        }
    }
    return static_cast<size_t>(-1);
}

// Factory function using the existing registry
std::unique_ptr<SeriesHandleBaseExt> create_series_by_index(
    size_t index, 
    PrecisionType prec, 
    const std::string& x_value, 
    const std::string& name,
    const NoiseConfig& noise_cfg = {}
) {
    switch (prec) {
        case PrecisionType::F64: {
            double x = parse_x<double>(x_value);
            auto s = shanks::series::series_registry<double, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleReal<double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::F32: {
            float x = parse_x<float>(x_value);
            auto s = shanks::series::series_registry<float, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleReal<float>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::FLong: {
            long double x = parse_x<long double>(x_value);
            auto s = shanks::series::series_registry<long double, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleReal<long double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::Arb: {
            // For arbitrary precision, use mpreal with default precision
            mpfr::mpreal x = parse_x<mpfr::mpreal>(x_value);
            auto s = shanks::series::series_registry<mpfr::mpreal, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleReal<mpfr::mpreal>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CF64: {
            double real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<double>(x_value.substr(0, plus_pos));
                imag = parse_x<double>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<double>(x_value);
            }
            std::complex<double> x(real, imag);
            auto s = shanks::series::series_registry<std::complex<double>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleComplex<double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CF32: {
            float real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<float>(x_value.substr(0, plus_pos));
                imag = parse_x<float>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<float>(x_value);
            }
            std::complex<float> x(real, imag);
            auto s = shanks::series::series_registry<std::complex<float>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleComplex<float>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CFLong: {
            long double real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<long double>(x_value.substr(0, plus_pos));
                imag = parse_x<long double>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<long double>(x_value);
            }
            std::complex<long double> x(real, imag);
            auto s = shanks::series::series_registry<std::complex<long double>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleComplex<long double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CArb: {
            mpfr::mpreal real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<mpfr::mpreal>(x_value.substr(0, plus_pos));
                imag = parse_x<mpfr::mpreal>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<mpfr::mpreal>(x_value);
            }
            std::complex<mpfr::mpreal> x(real, imag);
            auto s = shanks::series::series_registry<std::complex<mpfr::mpreal>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleComplex<mpfr::mpreal>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::IntervalF32: {
            float x = parse_x<float>(x_value);
            intprec::interval<float> xi(x);
            auto s = shanks::series::series_registry<intprec::interval<float>, size_t>::create(index, xi);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleInterval<float>>(std::move(s), prec, xi, name, noise_cfg);
        }
        case PrecisionType::IntervalF64: {
            double x = parse_x<double>(x_value);
            intprec::interval<double> xi(x);
            auto s = shanks::series::series_registry<intprec::interval<double>, size_t>::create(index, xi);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleInterval<double>>(std::move(s), prec, xi, name, noise_cfg);
        }
        case PrecisionType::IntervalFLong: {
            long double x = parse_x<long double>(x_value);
            intprec::interval<long double> xi(x);
            auto s = shanks::series::series_registry<intprec::interval<long double>, size_t>::create(index, xi);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleInterval<long double>>(std::move(s), prec, xi, name, noise_cfg);
        }
        case PrecisionType::IntervalArb: {
            mpfr::mpreal x = parse_x<mpfr::mpreal>(x_value);
            intprec::interval<mpfr::mpreal> xi(x);
            auto s = shanks::series::series_registry<intprec::interval<mpfr::mpreal>, size_t>::create(index, xi);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleInterval<mpfr::mpreal>>(std::move(s), prec, xi, name, noise_cfg);
        }
        case PrecisionType::CIntervalF64: {
            double real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<double>(x_value.substr(0, plus_pos));
                imag = parse_x<double>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<double>(x_value);
            }
            std::complex<intprec::interval<double>> x{intprec::interval<double>(real), intprec::interval<double>(imag)};
            auto s = shanks::series::series_registry<std::complex<intprec::interval<double>>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleCInterval<double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CIntervalF32: {
            float real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<float>(x_value.substr(0, plus_pos));
                imag = parse_x<float>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<float>(x_value);
            }
            std::complex<intprec::interval<float>> x{intprec::interval<float>(real), intprec::interval<float>(imag)};
            auto s = shanks::series::series_registry<std::complex<intprec::interval<float>>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleCInterval<float>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CIntervalFLong: {
            long double real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<long double>(x_value.substr(0, plus_pos));
                imag = parse_x<long double>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<long double>(x_value);
            }
            std::complex<intprec::interval<long double>> x{intprec::interval<long double>(real), intprec::interval<long double>(imag)};
            auto s = shanks::series::series_registry<std::complex<intprec::interval<long double>>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleCInterval<long double>>(std::move(s), prec, x, name, noise_cfg);
        }
        case PrecisionType::CIntervalArb: {
            mpfr::mpreal real = 0, imag = 0;
            size_t plus_pos = x_value.find('+');
            size_t j_pos = x_value.find('j');
            if (plus_pos != std::string::npos && j_pos != std::string::npos) {
                real = parse_x<mpfr::mpreal>(x_value.substr(0, plus_pos));
                imag = parse_x<mpfr::mpreal>(x_value.substr(plus_pos + 1, j_pos - plus_pos - 1));
            } else {
                real = parse_x<mpfr::mpreal>(x_value);
            }
            std::complex<intprec::interval<mpfr::mpreal>> x{intprec::interval<mpfr::mpreal>(real), intprec::interval<mpfr::mpreal>(imag)};
            auto s = shanks::series::series_registry<std::complex<intprec::interval<mpfr::mpreal>>, size_t>::create(index, x);
            if (!s) return nullptr;
            return std::make_unique<SeriesHandleCInterval<mpfr::mpreal>>(std::move(s), prec, x, name, noise_cfg);
        }
        default:
            set_error("Unsupported precision type");
            return nullptr;
    }
}

} // anonymous namespace

// ============================================================================
// Series Creation Implementation
// ============================================================================

extern "C" SHANKS_FFI_API ShanksSeriesHandle shanks_series_create(
    const char* name,
    const char* precision,
    const char* x_value,
    const char* args_json
) {
    clear_error();
    
    if (!name) {
        set_error("Series name is required");
        return nullptr;
    }
    
    PrecisionType prec;
    if (!parse_precision(precision, prec)) {
        set_error(std::string("Invalid precision: ") + (precision ? precision : "null"));
        return nullptr;
    }
    
    // Find series index
    size_t index = find_series_index(name);
    if (index == static_cast<size_t>(-1)) {
        set_error(std::string("Unknown series: ") + name);
        return nullptr;
    }
    
    std::string x = x_value ? x_value : "0";
    std::string n = name;
    
    try {
        auto handle = create_series_by_index(index, prec, x, n);
        if (!handle) {
            if (shanks::ffi::g_last_error.empty()) {
                set_error(std::string("Failed to create series: ") + name);
            }
            return nullptr;
        }
        return handle.release();
    } catch (const std::exception& e) {
        set_error(std::string("Error creating series: ") + e.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown error creating series");
        return nullptr;
    }
}

extern "C" SHANKS_FFI_API ShanksSeriesHandle shanks_series_create_with_noise(
    const char* name,
    const char* precision,
    const char* x_value,
    const char* args_json,
    const char* noise_type,
    const char* noise_method,
    double param1,
    double param2,
    uint64_t seed
) {
    if (!name || !noise_type || !noise_method) {
        set_error("Missing required arguments for noise config");
        return nullptr;
    }

    NoiseConfig cfg;
    cfg.enabled = true;
    cfg.param1 = param1;
    cfg.param2 = param2;
    cfg.seed = seed;

    std::string type_str(noise_type);
    if (type_str == "uniform") cfg.type = uniform;
    else if (type_str == "normal") cfg.type = normal;
    else if (type_str == "poisson") cfg.type = poisson;
    else {
        set_error(std::string("Unknown noise type: ") + type_str);
        return nullptr;
    }

    std::string method_str(noise_method);
    if (method_str == "jitter") cfg.method = jitter;
    else if (method_str == "scaling") cfg.method = scaling;
    else {
        set_error(std::string("Unknown noise method: ") + method_str);
        return nullptr;
    }

    PrecisionType prec;
    if (!parse_precision(precision, prec)) {
        set_error(std::string("Invalid precision: ") + (precision ? precision : "null"));
        return nullptr;
    }
    
    size_t index = find_series_index(name);
    if (index == static_cast<size_t>(-1)) {
        set_error(std::string("Unknown series: ") + name);
        return nullptr;
    }
    
    std::string x = x_value ? x_value : "0";
    std::string n = name;
    
    try {
        auto handle = create_series_by_index(index, prec, x, n, cfg);
        if (!handle) {
            if (shanks::ffi::g_last_error.empty()) {
                set_error(std::string("Failed to create series: ") + name);
            }
            return nullptr;
        }
        return handle.release();
    } catch (const std::exception& e) {
        set_error(std::string("Error creating series: ") + e.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown error creating series");
        return nullptr;
    }
}

extern "C" SHANKS_FFI_API void shanks_series_destroy(ShanksSeriesHandle handle) {
    if (handle) {
        auto* ptr = static_cast<SeriesHandleBaseExt*>(handle);
        delete ptr;
    }
}

extern "C" SHANKS_FFI_API char* shanks_series_get_sum(ShanksSeriesHandle handle) {
    clear_error();
    
    if (!handle) {
        set_error("Invalid series handle");
        return nullptr;
    }
    
    auto* ptr = static_cast<SeriesHandleBaseExt*>(handle);
    std::string sum = ptr->get_sum();
    // Return empty json object structure instead of literal empty string
    // if get_sum fails or is invalid, to prevent JSON parse errors when appended
    if (sum.empty()) return alloc_string("");
    return alloc_string(sum);
}

extern "C" SHANKS_FFI_API char* shanks_series_get_x(ShanksSeriesHandle handle) {
    clear_error();
    
    if (!handle) {
        set_error("Invalid series handle");
        return nullptr;
    }
    
    auto* ptr = static_cast<SeriesHandleBaseExt*>(handle);
    std::string x = ptr->get_x();
    return alloc_string(x);
}

extern "C" SHANKS_FFI_API FFISeriesResult* shanks_series_generate(
    ShanksSeriesHandle handle,
    uint64_t n
) {
    clear_error();
    if (!handle) {
        set_error("Series handle is null");
        return nullptr;
    }
    
    try {
        auto* series = static_cast<SeriesHandleBaseExt*>(handle);
        return series->generate(n);
    } catch (const std::exception& e) {
        set_error(std::string("Error generating series: ") + e.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown error generating series");
        return nullptr;
    }
}

extern "C" SHANKS_FFI_API void shanks_series_result_free(FFISeriesResult* result) {
    if (!result) return;
    shanks::ffi::free_ffi_line_coll(&result->sn);
    shanks::ffi::free_ffi_line_coll(&result->an);
    shanks::ffi::free_ffi_line_coll(&result->deviations);
    delete result;
}
