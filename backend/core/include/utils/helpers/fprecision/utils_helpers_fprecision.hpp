#ifndef UTILS_HELPERS_FPRECISION_HPP
#define UTILS_HELPERS_FPRECISION_HPP
#pragma once

#ifdef INC_FPRECISION
template <>
struct utils::helpers<arb::float_precision> {
    static arb::float_precision nextafter(const arb::float_precision& x, const arb::float_precision& y);
    static arb::float_precision fmod(const arb::float_precision& x, const arb::float_precision& y);
    static arb::float_precision floor(const arb::float_precision& x);
    static arb::float_precision ceil(const arb::float_precision& x);
    static arb::float_precision get_nan();
    static arb::float_precision frexp(const arb::float_precision& x, int* exp);
    static std::size_t get_precision(const arb::float_precision& x);
    static std::string to_string(const arb::float_precision& x);
    static bool isfinite(const arb::float_precision& x);
    static bool isnan(const arb::float_precision& x);
    static bool isinf(const arb::float_precision& x);
    static arb::float_precision epsilon(std::size_t precision = std::size_t{0});
    static arb::float_precision numeric_max(std::size_t precision = std::size_t{0});
};

inline arb::float_precision utils::helpers<arb::float_precision>::nextafter(const arb::float_precision& x,
                                                                            const arb::float_precision& y) {
    return arb::nextafter(x, y);
}

inline arb::float_precision utils::helpers<arb::float_precision>::fmod(const arb::float_precision& x,
                                                                       const arb::float_precision& y) {
    return arb::fmod(x, y);
}
inline arb::float_precision utils::helpers<arb::float_precision>::floor(const arb::float_precision& x) {
    return arb::floor(x);
}
inline arb::float_precision utils::helpers<arb::float_precision>::ceil(const arb::float_precision& x) {
    return arb::ceil(x);
}

inline arb::float_precision utils::helpers<arb::float_precision>::get_nan() { return arb::FP_QUIET_NAN; }

inline arb::float_precision utils::helpers<arb::float_precision>::frexp(const arb::float_precision& x, int* exp) {
    return arb::frexp(x, exp);
}

inline std::size_t utils::helpers<arb::float_precision>::get_precision(const arb::float_precision& x) {
    return x.precision();
}

inline std::string utils::helpers<arb::float_precision>::to_string(const arb::float_precision& x) {
    return x.toString();
}

inline bool utils::helpers<arb::float_precision>::isfinite(const arb::float_precision& x) { return arb::isfinite(x); }
inline bool utils::helpers<arb::float_precision>::isnan(const arb::float_precision& x) { return arb::isnan(x); }
inline bool utils::helpers<arb::float_precision>::isinf(const arb::float_precision& x) { return arb::isinf(x); }

inline arb::float_precision utils::helpers<arb::float_precision>::epsilon(std::size_t precision) {
    if (precision > 0)
        return arb::float_precision(0, precision).epsilon();
    else
        return arb::float_precision().epsilon();
}

inline arb::float_precision utils::helpers<arb::float_precision>::numeric_max([[maybe_unused]] std::size_t precision) {
    return arb::FP_INFINITY;
}

#endif

#endif