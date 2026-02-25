/**
 * @file ffi_internal.hpp
 * @brief Internal types shared between FFI implementation files.
 * @authors Sobolev Y. A.
 */

#ifndef FFI_INTERNAL_HPP
#define FFI_INTERNAL_HPP
#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <cmath>
#include <complex>
// Ensure mpreal is available before using it in specialization
#include "../../core/include/lib.hpp"

namespace shanks {
namespace ffi {

// Precision types enumeration
enum class PrecisionType {
    F32, F64, FLong, Arb,
    CF32, CF64, CFLong, CArb,
    IntervalF32, IntervalF64, IntervalFLong, IntervalArb,
    CIntervalF32, CIntervalF64, CIntervalFLong, CIntervalArb
};

// Parse precision string to enum
inline bool parse_precision(const char* precision, PrecisionType& out) {
    if (!precision) return false;
    
    if (strcmp(precision, "F32") == 0) { out = PrecisionType::F32; return true; }
    if (strcmp(precision, "F64") == 0) { out = PrecisionType::F64; return true; }
    if (strcmp(precision, "FLong") == 0) { out = PrecisionType::FLong; return true; }
    
    if (strncmp(precision, "Arb", 3) == 0) {
        out = PrecisionType::Arb;
        if (precision[3] != '\0') {
            try {
                int bits = std::stoi(precision + 3);
                if (bits > 0) mpfr::mpreal::set_default_prec(bits);
            } catch (...) {}
        }
        return true;
    }
    
    if (strcmp(precision, "CF32") == 0) { out = PrecisionType::CF32; return true; }
    if (strcmp(precision, "CF64") == 0) { out = PrecisionType::CF64; return true; }
    if (strcmp(precision, "CFLong") == 0) { out = PrecisionType::CFLong; return true; }
    
    if (strncmp(precision, "CArb", 4) == 0) {
        out = PrecisionType::CArb;
        if (precision[4] != '\0') {
            try {
                int bits = std::stoi(precision + 4);
                if (bits > 0) mpfr::mpreal::set_default_prec(bits);
            } catch (...) {}
        }
        return true;
    }
    
    if (strcmp(precision, "IntervalF32") == 0) { out = PrecisionType::IntervalF32; return true; }
    if (strcmp(precision, "IntervalF64") == 0) { out = PrecisionType::IntervalF64; return true; }
    if (strcmp(precision, "IntervalFLong") == 0) { out = PrecisionType::IntervalFLong; return true; }
    
    if (strncmp(precision, "IntervalArb", 11) == 0) {
        out = PrecisionType::IntervalArb;
        if (precision[11] != '\0') {
            try {
                int bits = std::stoi(precision + 11);
                if (bits > 0) mpfr::mpreal::set_default_prec(bits);
            } catch (...) {}
        }
        return true;
    }
    
    if (strcmp(precision, "CIntervalF32") == 0) { out = PrecisionType::CIntervalF32; return true; }
    if (strcmp(precision, "CIntervalF64") == 0) { out = PrecisionType::CIntervalF64; return true; }
    if (strcmp(precision, "CIntervalFLong") == 0) { out = PrecisionType::CIntervalFLong; return true; }
    
    if (strncmp(precision, "CIntervalArb", 12) == 0) {
        out = PrecisionType::CIntervalArb;
        if (precision[12] != '\0') {
            try {
                int bits = std::stoi(precision + 12);
                if (bits > 0) mpfr::mpreal::set_default_prec(bits);
            } catch (...) {}
        }
        return true;
    }
    
    return false;
}

// Check if precision is complex
inline bool is_complex_precision(PrecisionType p) {
    return p >= PrecisionType::CF32;
}

// Scientific notation representation for JSON output
struct ScientificValue {
    double mantissa;
    int64_t exponent;
    
    ScientificValue(double m = 0.0, int64_t e = 0) : mantissa(m), exponent(e) {}
    
    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\"mantissa\": " << mantissa << ", \"exponent\": " << exponent << "}";
        return oss.str();
    }
};

// Convert a value to scientific notation
template <typename T>
inline ScientificValue to_scientific(T value) {
    if (value == T(0)) {
        return ScientificValue(0.0, 0);
    }
    
    double abs_val = std::abs(static_cast<double>(value));
    int64_t exp = static_cast<int64_t>(std::floor(std::log10(abs_val)));
    double mantissa = static_cast<double>(value) / std::pow(10.0, exp);
    
    return ScientificValue(mantissa, exp);
}

// Specialization for mpfr::mpreal (if needed, otherwise we rely on cast)
template <>
inline ScientificValue to_scientific(mpfr::mpreal value) {
    if (value == 0) {
        return ScientificValue(0.0, 0);
    }
    
    std::ostringstream oss;
    oss << value;
    std::string s = oss.str();
    
    // Parse scientific notation from mpreal string
    size_t e_pos = s.find('e');
    if (e_pos != std::string::npos) {
        double mantissa = std::stod(s.substr(0, e_pos));
        int64_t exp = std::stoll(s.substr(e_pos + 1));
        return ScientificValue(mantissa, exp);
    }
    
    return ScientificValue(std::stod(s), 0);
}

// JSON helpers
inline std::string complex_to_json(double real, double imag) {
    std::ostringstream oss;
    oss << "{\"real\": " << to_scientific(real).to_json() 
        << ", \"imag\": " << to_scientific(imag).to_json() << "}";
    return oss.str();
}

template <typename T>
inline std::string interval_to_json(const intprec::interval<T>& value) {
    std::ostringstream oss;
    oss << "{\"inf\": " << to_scientific(value.inf()).to_json() 
        << ", \"sup\": " << to_scientific(value.sup()).to_json() << "}";
    return oss.str();
}

template <typename T>
inline std::string complex_interval_to_json(const std::complex<intprec::interval<T>>& value) {
    std::ostringstream oss;
    oss << "{\"real\": " << interval_to_json(value.real()) 
        << ", \"imag\": " << interval_to_json(value.imag()) << "}";
    return oss.str();
}

// Helper to allocate and copy string for FFI return
inline char* alloc_string(const std::string& s) {
    if (s.empty()) {
        char* ptr = new char[1];
        ptr[0] = '\0';
        return ptr;
    }
    char* ptr = new char[s.size() + 1];
    std::memcpy(ptr, s.c_str(), s.size() + 1);
    return ptr;
}

// Thread-local error storage
extern thread_local std::string g_last_error;

inline void set_error(const std::string& msg) {
    g_last_error = msg;
}

inline void clear_error() {
    g_last_error.clear();
}

// Type-erased series handle base class
struct SeriesHandleBase {
    virtual ~SeriesHandleBase() = default;
    virtual std::string generate(uint64_t n, bool enable_profiling) = 0;
    virtual PrecisionType get_precision() const = 0;
    virtual std::string get_name() const = 0;
    virtual const void* get_raw_data(uint64_t n) const = 0;
    virtual const void* get_native_sum() const = 0;
};

// Type-erased acceleration handle base class
struct AccelHandleBase {
    virtual ~AccelHandleBase() = default;
    virtual std::string apply(
        SeriesHandleBase* series, 
        uint64_t n, 
        uint64_t order, 
        bool enable_profiling
    ) = 0;
    virtual PrecisionType get_precision() const = 0;
    virtual std::string get_name() const = 0;
};

} // namespace ffi
} // namespace shanks

#endif // FFI_INTERNAL_HPP
