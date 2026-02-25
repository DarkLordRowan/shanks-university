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
    
    std::string generate(uint64_t n, bool enable_profiling) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return "{}";
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            result = apply_noise<T, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        std::ostringstream oss;
        oss << "{\"Sn\": [";
        for (size_t i = 0; i < result.Sn.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::to_scientific(result.Sn[i]).to_json();
        }
        oss << "], \"an\": [";
        for (size_t i = 0; i < result.an.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::to_scientific(result.an[i]).to_json();
        }
        oss << "]";
        
        // Add sum if available
        try {
            std::string sum_str;
            oss << ", \"sum\": \"" << sum_str << "\"";
        } catch (...) {}
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto counts = shanks::profiling::get_counts();
            oss << ", \"profiling\": {"
                << "\"add\": " << counts.add << ", "
                << "\"mul\": " << counts.mul << ", "
                << "\"div\": " << counts.div << ", "
                << "\"special\": " << counts.special << "}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
    }
    
    std::string get_sum() const override {
        if (!series) return "";
        try {
            T sum = series->get_sum();
            std::ostringstream oss;
            oss << sum;
            return oss.str();
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
    
    std::string generate(uint64_t n, bool enable_profiling) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return "{}";
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            result = apply_noise<std::complex<T>, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        std::ostringstream oss;
        oss << "{\"Sn\": [";
        for (size_t i = 0; i < result.Sn.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::complex_to_json(result.Sn[i].real(), result.Sn[i].imag());
        }
        oss << "], \"an\": [";
        for (size_t i = 0; i < result.an.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::complex_to_json(result.an[i].real(), result.an[i].imag());
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto counts = shanks::profiling::get_counts();
            oss << ", \"profiling\": {"
                << "\"add\": " << counts.add << ", "
                << "\"mul\": " << counts.mul << ", "
                << "\"div\": " << counts.div << ", "
                << "\"special\": " << counts.special << "}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
    }
    
    std::string get_sum() const override {
        if (!series) return "";
        try {
            auto sum = series->get_sum();
            std::ostringstream oss;
            oss << sum.real() << "+" << sum.imag() << "j";
            return oss.str();
        } catch (...) {
            return "";
        }
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
    
    std::string generate(uint64_t n, bool enable_profiling) override {
#ifdef SHANKS_ENABLE_PROFILING
        shanks::profiling::reset_counts();
#endif
        if (!series) {
            set_error("Series is null");
            return "{}";
        }
        
        auto result = series->generate(static_cast<size_t>(n));
        if (noise_cfg.enabled) {
            // Noise for intervals would require interval noise generation which might not be implemented yet.
            // Leaving as is if apply_noise supports intervals, otherwise it will just use the value.
            // Note: Currently apply_noise on interval may fallback to throwing or not working, 
            // but we can try letting it work or disabling noise.
            // result = apply_noise<intprec::interval<T>, double>(result, noise_cfg.method, noise_cfg.type, noise_cfg.seed, noise_cfg.param1, noise_cfg.param2);
        }
        
        std::ostringstream oss;
        oss << "{\"Sn\": [";
        for (size_t i = 0; i < result.Sn.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::interval_to_json(result.Sn[i]);
        }
        oss << "], \"an\": [";
        for (size_t i = 0; i < result.an.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << shanks::ffi::interval_to_json(result.an[i]);
        }
        oss << "]";
        
#ifdef SHANKS_ENABLE_PROFILING
        if (enable_profiling) {
            auto counts = shanks::profiling::get_counts();
            oss << ", \"profiling\": {"
                << "\"add\": " << counts.add << ", "
                << "\"mul\": " << counts.mul << ", "
                << "\"div\": " << counts.div << ", "
                << "\"special\": " << counts.special << "}";
            shanks::profiling::reset_counts();
        }
#endif
        
        oss << "}";
        return oss.str();
    }
    
    std::string get_sum() const override {
        if (!series) return "";
        try {
            auto sum = series->get_sum();
            std::ostringstream oss;
            oss << "[" << sum.inf() << ", " << sum.sup() << "]";
            return oss.str();
        } catch (...) {
            return "";
        }
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
            // Complex double - parse "real+imagj" format
            double real = 0, imag = 0;
            // Simple parsing: assume format "real+imagj" or just "real"
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
            set_error(std::string("Failed to create series: ") + name);
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
            set_error(std::string("Failed to create series: ") + name);
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

extern "C" SHANKS_FFI_API char* shanks_series_generate(
    ShanksSeriesHandle handle,
    uint64_t n,
    int enable_profiling
) {
    clear_error();
    
    if (!handle) {
        set_error("Invalid series handle");
        return alloc_string("{}");
    }
    
    auto* ptr = static_cast<SeriesHandleBaseExt*>(handle);
    try {
        std::string result = ptr->generate(n, enable_profiling != 0);
        return alloc_string(result);
    } catch (const std::exception& e) {
        set_error(std::string("Error generating series: ") + e.what());
        return alloc_string("{}");
    }
}
