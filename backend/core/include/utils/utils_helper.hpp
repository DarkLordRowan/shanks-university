#ifndef UTILS_HELPER_H
#define UTILS_HELPER_H
#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <type_traits>

#include "utils_base.hpp"

/**
 * @file utils_helper.hpp
 * @brief This file contains various helper utilities like string conversion and finiteness checks.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Converts variables of various types to std::string
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return std::string representation
 */
template <typename T>
std::string utils::to_string(const T& x) {
    // Formatting based on type properties and library support
    if constexpr (std::is_floating_point<T>::value) {
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(std::numeric_limits<T>::max_digits10) << x;
        return oss.str();
    } else if constexpr (std::is_integral<T>::value) {
        return std::to_string(x);
    }
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        char* s = nullptr;
        // bits2digits(b) = floor(b * log10(2))
        // We add small buffer to ensure all bits are uniquely represented
        int digits = mpfr::bits2digits(x.getPrecision()) + 2;
        if (mpfr_asprintf(&s, "%.*RNg", digits, x.mpfr_srcptr()) >= 0) {
            std::string res(s);
            mpfr_free_str(s);
            return res;
        }
        return "conversion error";
    }
#endif
    else if constexpr (is_complex_custom<T>::value || is_complex_t<T>::value)
        return "( " + utils::to_string(x.real()) + ", " + utils::to_string(x.imag()) + ")";
    else {
        if constexpr (requires { x.value; }) {
            return utils::to_string(x.value);
        } else if constexpr (AcceptedLike<T>) {
            using std::to_string;
            return to_string(x);
        } else {
            static_assert(dependent_false<T>::value, "utils::to_string not implemented for this type");
        }
    }
}

/**
 * @brief Checks if a variable represents a finite value
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return bool (is finite)
 */
template <typename T>
bool utils::isfinite(const T& x) {
    using std::isfinite;
    // Using standard or library-specific checks for finiteness
    if constexpr (std::is_floating_point<T>::value)
        return std::isfinite(x);
    else if constexpr (is_complex_t<T>::value)
        return utils::isfinite(x.real()) && utils::isfinite(x.imag());
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return mpfr::isfinite(x) && !mpfr::isnan(x) && !mpfr::isinf(x);
    }
#endif
    else if constexpr (is_complex_custom<T>::value) {
        return utils::isfinite(x.real()) && utils::isfinite(x.imag());
    } else {
        if constexpr (AcceptedLike<T>) {
            return isfinite(x);
        } else {
            static_assert(dependent_false<T>::value, "utils::isfinite not implemented for this type");
        }
    }
}

/**
 * @brief Returns the machine epsilon for type T
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (epsilon value)
 */
template <typename T>
T utils::epsilon(const T& x) {
    // Selecting the epsilon calculation method for the given type
    if constexpr (is_standard_types<T>::value) return std::numeric_limits<T>::epsilon();
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return mpfr::machine_epsilon(x);
    }
#endif
    else {
        if constexpr (AcceptedLike<T>) {
            return std::numeric_limits<T>::epsilon();
        } else {
            static_assert(dependent_false<T>::value, "utils::epsilon not implemented for this type");
        }
    }
}

template <typename T>
T utils::numeric_max(size_t precision) {
    if constexpr (is_standard_types<T>::value)
        return std::numeric_limits<T>::max();
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return std::numeric_limits<mpfr::mpreal>::max(precision);
    } else if constexpr (std::is_same<T, std::complex<mpfr::mpreal>>::value) {
        return utils::numeric_max<mpfr::mpreal>(precision);
    } else {
        if constexpr (AcceptedLike<T>) {
            return std::numeric_limits<T>::max();
        } else {
            static_assert(dependent_false<T>::value, "utils::numeric_max not implemented for this type");
        }
    }
}

#endif
