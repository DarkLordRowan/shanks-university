#ifndef UTILS_HELPER_H
#define UTILS_HELPER_H
#pragma once

template <FloatLike T>
T utils::nextafter(const T& x, const T& y) {
    if constexpr (std::is_same<T, float>::value)
        return std::nextafterf(x, y);
    else if constexpr (std::is_same<T, double>::value)
        return std::nextafter(x, y);
    else if constexpr (std::is_same<T, long double>::value)
        return std::nextafterl(x, y);
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value)
        return mpfr::nextafter(x, y);
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::get_nan not implemented for this type");
    }
}

template <FloatLike T>
T utils::fmod(const T& x, const T& y) {
    if constexpr (std::is_same<T, float>::value)
        return std::fmodf(x, y);
    else if constexpr (std::is_same<T, double>::value)
        return std::fmod(x, y);
    else if constexpr (std::is_same<T, long double>::value)
        return std::fmodl(x, y);
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value)
        return mpfr::fmod(x, y);
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::get_nan not implemented for this type");
    }
}

template <FloatLike T>
T utils::floor(const T& x) {
    if constexpr (std::is_same<T, float>::value)
        return std::floorf(x);
    else if constexpr (std::is_same<T, double>::value)
        return std::floor(x);
    else if constexpr (std::is_same<T, long double>::value)
        return std::floorl(x);
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value)
        return mpfr::floor(x);
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::get_nan not implemented for this type");
    }
}

template <FloatLike T>
T utils::ceil(const T& x) {
    if constexpr (std::is_same<T, float>::value)
        return std::ceilf(x);
    else if constexpr (std::is_same<T, double>::value)
        return std::ceil(x);
    else if constexpr (std::is_same<T, long double>::value)
        return std::ceill(x);
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value)
        return mpfr::ceil(x);
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::get_nan not implemented for this type");
    }
}

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
    if constexpr (std::is_floating_point<T>::value || std::is_integral<T>::value) return std::to_string(x);
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
#define MAX_PRECISION_AVAILABLE -1
        return x.toString(MAX_PRECISION_AVAILABLE, 10);
    }
#endif
    else if constexpr (is_complex_custom<T>::value || is_complex_t<T>::value)
        return "( " + utils::to_string(x.real()) + ", " + utils::to_string(x.imag()) + ")";
    else if constexpr (is_interval<T>::value)
        return x.toString();
    else {
        static_assert(dependent_false<T>::value, "utils::to_string not implemented for this type");
    }
}

template <FloatLike T>
T utils::get_nan(const T& x) {
    if constexpr (std::is_same<T, float>::value)
        return std::nanf("");
    else if constexpr (std::is_same<T, double>::value)
        return std::nan("");
    else if constexpr (std::is_same<T, long double>::value)
        return std::nanl("");
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return mpfr::mpreal(0.0, utils::get_precision(x)).setNan();
    }
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::get_nan not implemented for this type");
    }
}

template <AcceptedLike T>
bool utils::isnan(const T& x) {
    if constexpr (std::is_floating_point<T>::value || std::is_integral<T>::value)
        return std::isnan(x);
    else if constexpr (is_complex_t<T>::value || is_complex_custom<T>::value)
        return utils::isnan(x.real()) || utils::isnan(x.imag());
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return mpfr::isnan(x);
    }
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::isnan not implemented for this type");
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
    // Using standard or library-specific checks for finiteness
    if constexpr (std::is_floating_point<T>::value)
        return std::isfinite(x);
    else if constexpr (isComplexLike<T>::value)
        return utils::isfinite(x.real()) && utils::isfinite(x.imag());
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return mpfr::isfinite(x);
    }
#endif
    else if constexpr (is_interval<T>::value)
        return utils::isfinite(x.leftinterval()) && utils::isfinite(x.rightinterval());
    else {
        static_assert(dependent_false<T>::value, "utils::isfinite not implemented for this type");
    }
}

template <typename T>
bool utils::isinf(const T& x) {
    if constexpr (is_standard_types<T>::value)
        return std::isinf(x);
    else if constexpr (is_complex_custom<T>::value)
        return utils::isinf(x.real()) || utils::isinf(x.imag());
    else if constexpr (is_interval<T>::value)
        return utils::isinf(x.leftinterval()) || utils::isinf(x.rightinterval());
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value)
        return mpfr::isinf(x);
#endif
    else {
        static_assert(dependent_false<T>::value, "utils::isinf not implemented for this type");
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
    else if constexpr (is_interval<T>::value)
        return utils::cast<T>(utils::epsilon(x.leftinterval()));
    else {
        static_assert(dependent_false<T>::value, "utils::epsilon not implemented for this type");
    }
}

template <typename T>
T utils::numeric_max(size_t precision) {
    if constexpr (is_standard_types<T>::value)
        return std::numeric_limits<T>::max();
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return std::numeric_limits<mpfr::mpreal>::max(mpfr::digits2bits(precision));
    } else if constexpr (std::is_same<T, std::complex<mpfr::mpreal>>::value) {
        return utils::numeric_max<mpfr::mpreal>(mpfr::digits2bits(precision));
    } else if constexpr (is_interval<T>::value) {
        return utils::cast<T>(utils::numeric_max<typename T::value_type>(precision));
    } else {
        static_assert(dependent_false<T>::value, "utils::numeric_max not implemented for this type");
    }
}

#endif