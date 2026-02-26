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
    
    double val_d = static_cast<double>(value);
    if (std::isnan(val_d)) {
        return ScientificValue(0.0, 0);
    }
    if (std::isinf(val_d)) {
        return ScientificValue(val_d > 0 ? 1.7976931348623157 : -1.7976931348623157, 308);
    }

    double abs_val = std::abs(val_d);
    int64_t exp = static_cast<int64_t>(std::floor(std::log10(abs_val)));
    double mantissa = val_d / std::pow(10.0, exp);
    
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
    
    if (s.find("NaN") != std::string::npos || s.find("nan") != std::string::npos) {
        return ScientificValue(0.0, 0);
    }
    if (s.find("Inf") != std::string::npos || s.find("inf") != std::string::npos) {
        bool neg = s.find("-") != std::string::npos;
        return ScientificValue(neg ? -1.7976931348623157 : 1.7976931348623157, 308);
    }
    
    // Parse scientific notation from mpreal string
    size_t e_pos = s.find('e');
    if (e_pos != std::string::npos) {
        double mantissa = std::stod(s.substr(0, e_pos));
        int64_t exp = std::stoll(s.substr(e_pos + 1));
        return ScientificValue(mantissa, exp);
    }
    
    return ScientificValue(std::stod(s), 0);
}

// ============================================================================
// Binary FFI Builders
// ============================================================================

struct BinarySerializer {
    std::vector<double> mantissas;
    std::vector<int64_t> exponents;

    BinarySerializer() {
        mantissas.reserve(1000);
        exponents.reserve(1000);
    }

    template<typename T>
    void push(T val) {
        auto sci = shanks::ffi::to_scientific(val);
        mantissas.push_back(sci.mantissa);
        exponents.push_back(sci.exponent);
    }

    void push_scientific(const ScientificValue& sci) {
        mantissas.push_back(sci.mantissa);
        exponents.push_back(sci.exponent);
    }

    FFILine finalize() {
        FFILine line;
        line.len = mantissas.size();
        if (line.len > 0) {
            line.mantissas = new double[line.len];
            line.exponents = new int64_t[line.len];
            std::memcpy(line.mantissas, mantissas.data(), line.len * sizeof(double));
            std::memcpy(line.exponents, exponents.data(), line.len * sizeof(int64_t));
        } else {
            line.mantissas = nullptr;
            line.exponents = nullptr;
        }
        return line;
    }
};

template<typename T>
struct RealBinarySerializer {
    BinarySerializer ser;

    void push(T val) { ser.push(val); }

    FFILineColl finalize() {
        FFILineColl coll;
        coll.type = 0; // Real
        coll.lines[0] = ser.finalize();
        coll.lines[1] = {nullptr, nullptr, 0};
        coll.lines[2] = {nullptr, nullptr, 0};
        coll.lines[3] = {nullptr, nullptr, 0};
        return coll;
    }
};

template<typename T>
struct ComplexBinarySerializer {
    BinarySerializer real;
    BinarySerializer imag;
    
    void push(const std::complex<T>& val) {
        real.push(val.real());
        imag.push(val.imag());
    }
    
    FFILineColl finalize() {
        FFILineColl coll;
        coll.type = 1; // Complex
        coll.lines[0] = real.finalize();
        coll.lines[1] = imag.finalize();
        coll.lines[2] = {nullptr, nullptr, 0};
        coll.lines[3] = {nullptr, nullptr, 0};
        return coll;
    }
};

template<typename T>
struct IntervalBinarySerializer {
    BinarySerializer inf;
    BinarySerializer sup;
    
    void push(const intprec::interval<T>& val) {
        inf.push(val.inf());
        sup.push(val.sup());
    }
    
    FFILineColl finalize() {
        FFILineColl coll;
        coll.type = 2; // Interval
        coll.lines[0] = inf.finalize();
        coll.lines[1] = sup.finalize();
        coll.lines[2] = {nullptr, nullptr, 0};
        coll.lines[3] = {nullptr, nullptr, 0};
        return coll;
    }
};

template<typename T>
struct CIntervalBinarySerializer {
    BinarySerializer real_inf;
    BinarySerializer real_sup;
    BinarySerializer imag_inf;
    BinarySerializer imag_sup;
    
    void push(const std::complex<intprec::interval<T>>& val) {
        real_inf.push(val.real().inf());
        real_sup.push(val.real().sup());
        imag_inf.push(val.imag().inf());
        imag_sup.push(val.imag().sup());
    }
    
    FFILineColl finalize() {
        FFILineColl coll;
        coll.type = 3; // CInterval
        coll.lines[0] = real_inf.finalize();
        coll.lines[1] = real_sup.finalize();
        coll.lines[2] = imag_inf.finalize();
        coll.lines[3] = imag_sup.finalize();
        return coll;
    }
};

// Memory free helpers
inline void free_ffi_line(FFILine* line) {
    if (line->mantissas) {
        delete[] line->mantissas;
        line->mantissas = nullptr;
    }
    if (line->exponents) {
        delete[] line->exponents;
        line->exponents = nullptr;
    }
    line->len = 0;
}

inline void free_ffi_line_coll(FFILineColl* coll) {
    for (int i = 0; i < 4; i++) {
        free_ffi_line(&coll->lines[i]);
    }
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
    virtual FFISeriesResult* generate(uint64_t n) = 0;
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
