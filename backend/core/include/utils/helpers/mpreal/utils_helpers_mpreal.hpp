#ifndef UTILS_HELPERS_MPREAL_HPP
#define UTILS_HELPERS_MPREAL_HPP
#pragma once

#ifdef __MPREAL_H__

template <>
struct utils::helpers<mpfr::mpreal> {
    static mpfr::mpreal nextafter(const mpfr::mpreal& x, const mpfr::mpreal& y);
    static mpfr::mpreal fmod(const mpfr::mpreal& x, const mpfr::mpreal& y);
    static mpfr::mpreal floor(const mpfr::mpreal& x);
    static mpfr::mpreal ceil(const mpfr::mpreal& x);
    static mpfr::mpreal get_nan();
    static mpfr::mpreal frexp(const mpfr::mpreal& x, int* exp);
    static std::size_t get_precision(const mpfr::mpreal& x);
    static std::string to_string(const mpfr::mpreal& x);
    static bool isfinite(const mpfr::mpreal& x);
    static bool isnan(const mpfr::mpreal& x);
    static bool isinf(const mpfr::mpreal& x);
    static mpfr::mpreal epsilon(std::size_t precision = std::size_t{0});
    static mpfr::mpreal numeric_max(std::size_t precision = std::size_t{0});
};

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::nextafter(const mpfr::mpreal& x, const mpfr::mpreal& y) {
    return mpfr::nextafter(x, y);
}

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::fmod(const mpfr::mpreal& x, const mpfr::mpreal& y) {
    return mpfr::fmod(x, y);
}
inline mpfr::mpreal utils::helpers<mpfr::mpreal>::floor(const mpfr::mpreal& x) { return mpfr::floor(x); }
inline mpfr::mpreal utils::helpers<mpfr::mpreal>::ceil(const mpfr::mpreal& x) { return mpfr::ceil(x); }

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::get_nan() {
    return mpfr::mpreal().setNan();
}

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::frexp(const mpfr::mpreal& x, int* exp){
    return mpfr::frexp(x, exp);
}

inline std::size_t utils::helpers<mpfr::mpreal>::get_precision(const mpfr::mpreal& x) {
    return mpfr::bits2digits(x.get_prec());
}

inline std::string utils::helpers<mpfr::mpreal>::to_string(const mpfr::mpreal& x) { return x.toString(-1, 10); }

inline bool utils::helpers<mpfr::mpreal>::isfinite(const mpfr::mpreal& x) { return mpfr::isfinite(x); }
inline bool utils::helpers<mpfr::mpreal>::isnan(const mpfr::mpreal& x) { return mpfr::isnan(x); }
inline bool utils::helpers<mpfr::mpreal>::isinf(const mpfr::mpreal& x) { return mpfr::isinf(x); }

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::epsilon(std::size_t precision) {
    if (precision > 0)
        return mpfr::machine_epsilon(precision);
    else
        return mpfr::machine_epsilon();
}

inline mpfr::mpreal utils::helpers<mpfr::mpreal>::numeric_max(std::size_t precision) {
    if (precision > 0)
        return std::numeric_limits<mpfr::mpreal>::max(mpfr::digits2bits(precision));
    else
        return std::numeric_limits<mpfr::mpreal>::max();
}

#endif

#endif