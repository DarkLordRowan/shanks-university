#ifndef CUSTOM_CONCEPTS_HPP
#define CUSTOM_CONCEPTS_HPP
#pragma once

#ifndef __MPREAL_H__
    #include "../libs/mpfr/mpreal.h"
#endif

#ifdef _CL_FLOAT_CLASS_H
    #include <cln/float_class.h>
#endif

#ifdef INC_FPRECISION
    #include "../libs/arbitrary_arithmetics/fprecision.h"
#endif

#ifdef INC_COMPLEXPRECISION
    #include "../libs/arbitrary_arithmetics/complexprecision.h"
#endif

#ifdef INC_INTERVALPRECISION
    #include "../libs/arbitrary_arithmetics/intervalprecision.h"
#endif

#include <type_traits>

template<typename T>
struct isFloatLike : std::integral_constant<bool,
std::is_floating_point<T>::value 
#ifdef INC_FPRECISION
|| std::is_same<T, float_precision>::value
#endif
#ifdef _CL_FLOAT_CLASS_H
|| std::is_same<T, cln::cl_F>::value
|| std::is_same<T, cln::cl_R>::value
#endif
#ifdef __MPREAL_H__
|| std::is_same<T, mpfr::mpreal>::value
#endif
>{};

template<typename T>
concept FloatLike =
std::is_floating_point<T>::value 
#ifdef INC_FPRECISION
|| std::is_same<T, float_precision>::value
#endif
#ifdef _CL_FLOAT_CLASS_H
|| std::is_same<T, cln::cl_F>::value
|| std::is_same<T, cln::cl_R>::value
#endif
#ifdef __MPREAL_H__
|| std::is_same<T, mpfr::mpreal>::value
#endif
;

/**
 * @brief Is complex with std::floating_point value_type
 * @tparam T 
 */
template<typename T>
struct is_complex_t : public std::false_type {};

template<std::floating_point U>
struct is_complex_t<std::complex<U>> : public std::true_type {};

/**
 * @brief Is complex with not only std::floating_point, but also for FloatLike types, such as mpfr::mpreal or similar ones
 * @tparam T 
 */
template<typename T>
struct is_complex_custom : public std::false_type {};

template<FloatLike U>
struct is_complex_custom<std::complex<U>> : public std::true_type {};

/**
 * @brief Does the type is from std:: or custom composite type has std:: value_type
 * @tparam T 
 */
template<typename T>
struct is_standard_types : std::integral_constant<bool,
    std::is_floating_point<T>::value || is_complex_t<T>::value || std::is_integral<T>::value
    #ifdef INC_COMPLEXPRECISION
    || 
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value
    #endif
    #ifdef INC_INTERVALPRECISION
    ||
    std::is_same<T, interval<float>>::value  ||
    std::is_same<T, interval<double>>::value ||
    std::is_same<T, interval<long double>>::value
    #endif
>{};

/**
 * @brief Does the type has precision capabilities
 * @tparam T 
 */
template<typename T>
struct is_precisable : std::integral_constant<bool,
    false
    #ifdef INC_FPRECISION
    || std::is_same<T, float_precision>::value
    #ifdef INC_COMPLEXPRECISION
    || std::is_same<T, complex_precision<float_precision>>::value
    #endif
    #ifdef INC_INTERVALPRECISION
    || std::is_same<T, interval<float_precision>>::value
    #endif
    #endif
    #ifdef _CL_FLOAT_CLASS_H
    || std::is_same<T, cln::cl_F>::value
    || std::is_same<T, cln::cl_R>::value
    #endif
    #ifdef __MPREAL_H__
    || std::is_same<T, mpfr::mpreal>::value
    || std::is_same<T, std::complex<mpfr::mpreal>>::value
    #endif
>{};

template<typename T>
struct isComplexLike : std::integral_constant<bool,
    is_complex_t<T>::value
    #ifdef INC_COMPLEXPRECISION
    ||
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value ||
    #ifdef INC_FPRECISION
        std::is_same<T, complex_precision<float_precision>>::value
    #endif
    #endif
    #ifdef __MPREAL_H__
    || std::is_same<T, std::complex<mpfr::mpreal>>::value
    #endif
>{};


template<typename T>
concept ComplexLike = is_complex_t<T>::value
#ifdef INC_COMPLEXPRECISION
    ||
    #ifdef INC_FPRECISION
        std::is_same<T, complex_precision<float_precision>>::value ||
    #endif
    std::is_same<T, complex_precision<float>>::value  ||
    std::is_same<T, complex_precision<double>>::value ||
    std::is_same<T, complex_precision<long double>>::value
#endif
#ifdef __MPREAL_H__
    || std::is_same<T, std::complex<mpfr::mpreal>>::value
#endif
;

template<typename T>
struct is_interval : std::integral_constant<bool,
    false 
    #ifdef INC_INTERVALPRECISION
    ||
    #ifdef INC_FPRECISION
        std::is_same<T, interval<float_precision>>::value ||
    #endif
    std::is_same<T, interval<float>>::value  ||
    std::is_same<T, interval<double>>::value ||
    std::is_same<T, interval<long double>>::value
    #endif
>{};


template<typename T>
concept IntervalLike = false
#ifdef INC_INTERVALPRECISION
    ||
    #ifdef INC_FPRECISION
        std::is_same<T, interval<float_precision>>::value ||
    #endif
    std::is_same<T, interval<float>>::value  ||
    std::is_same<T, interval<double>>::value ||
    std::is_same<T, interval<long double>>::value
#endif
;

/**
 * @brief Does the type is FloatLike, or ComplexLike, or IntervalLike
 * @tparam T 
 */
template<typename T>
concept AcceptedLike = requires{ 
    requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; 
};

template<typename K>
concept UnsignedIntLike = requires {
    std::is_integral<K>::value && !std::is_signed<K>::value;
};

template<typename K>
struct isUnsignedIntLike : std::integral_constant<bool, std::is_integral<K>::value && !std::is_signed<K>::value>{};

#endif