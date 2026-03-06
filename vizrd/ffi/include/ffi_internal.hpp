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
#include "../../../backend/core/include/lib.hpp"

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
                int decimals = std::stoi(precision + 3);
                if (decimals > 0) mpfr::mpreal::set_default_prec(mpfr::digits2bits(decimals));
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
                int decimals = std::stoi(precision + 4);
                if (decimals > 0) mpfr::mpreal::set_default_prec(mpfr::digits2bits(decimals));
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
                int decimals = std::stoi(precision + 11);
                if (decimals > 0) mpfr::mpreal::set_default_prec(mpfr::digits2bits(decimals));
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
                int decimals = std::stoi(precision + 12);
                if (decimals > 0) mpfr::mpreal::set_default_prec(mpfr::digits2bits(decimals));
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
        // Return a large but finite value for infinity to avoid plot crashes,
        // using the 308 exponent as a signal for "huge".
        return ScientificValue(val_d > 0 ? 1.0 : -1.0, 308);
    }

    double abs_val = std::abs(val_d);
    // log10 handles the scale
    double l10 = std::log10(abs_val);
    int64_t exp = static_cast<int64_t>(std::floor(l10));
    
    // Safety check for exponent range to avoid pow() overflow/underflow
    if (exp < -300) {
        // Very small: normalize to -300
        double mantissa = val_d * std::pow(10.0, -exp - 300) / 1e300;
        return ScientificValue(mantissa, exp);
    } else if (exp > 300) {
        // Very large: normalize to 300
        double mantissa = (val_d / 1e300) / std::pow(10.0, exp - 300);
        return ScientificValue(mantissa, exp);
    }

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
    // Force scientific notation with sufficient precision
    oss << std::scientific << std::setprecision(18) << value;
    std::string s = oss.str();

    if (s.find("NaN") != std::string::npos || s.find("nan") != std::string::npos) {
        return ScientificValue(0.0, 0);
    }
    if (s.find("Inf") != std::string::npos || s.find("inf") != std::string::npos) {
        bool neg = s.find("-") != std::string::npos;
        return ScientificValue(neg ? -1.0 : 1.0, 308);
    }

    // Parse scientific notation from mpreal string (format: 1.23e+45)
    size_t e_pos = s.find('e');
    if (e_pos != std::string::npos) {
        try {
            double mantissa = std::stod(s.substr(0, e_pos));
            int64_t exp = std::stoll(s.substr(e_pos + 1));
            return ScientificValue(mantissa, exp);
        } catch (...) {
            // Fallback for weirdly formatted strings
            return ScientificValue(0.0, 0);
        }
    }

    try {
        return ScientificValue(std::stod(s), 0);
    } catch (...) {
        return ScientificValue(0.0, 0);
    }
}

// ============================================================================
// Type Traits for Bridge
// ============================================================================

template <typename T>
struct is_complex : std::false_type {};
template <typename T>
struct is_complex<std::complex<T>> : std::true_type {};

template <typename T>
struct is_interval : std::false_type {};
template <typename T>
struct is_interval<intprec::interval<T>> : std::true_type {};

template <typename T>
struct is_complex_interval : std::false_type {};
template <typename T>
struct is_complex_interval<std::complex<intprec::interval<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_complex_v = is_complex<T>::value;
template <typename T>
inline constexpr bool is_interval_v = is_interval<T>::value;
template <typename T>
inline constexpr bool is_complex_interval_v = is_complex_interval<T>::value;

} // namespace ffi
} // namespace shanks

#endif // FFI_INTERNAL_HPP
