#ifndef CUSTOM_CONCEPTS_HPP
#define CUSTOM_CONCEPTS_HPP
#pragma once

/**
 * @file custom_concepts.hpp
 * @brief This file contains the definitions of custom type traits and concepts used throughout the project.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */


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

/**
 * @brief Type trait to check if a type behaves like a floating-point number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to inspect.
 */
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

/**
 * @brief Concept for types that behave like floating-point numbers.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
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
 * @brief Type trait to check if a type is a standard complex type with floating-point components.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template<typename T>
struct is_complex_t : public std::false_type {};

template<std::floating_point U>
struct is_complex_t<std::complex<U>> : public std::true_type {};

/**
 * @brief Type trait to check if a type is complex with components satisfying FloatLike.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template<typename T>
struct is_complex_custom : public std::false_type {};

template<FloatLike U>
struct is_complex_custom<std::complex<U>> : public std::true_type {};

/**
 * @brief Type trait to check if a type is considered a "standard" project type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
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
 * @brief Type trait to check if a type supports explicit precision settings.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
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

/**
 * @brief Type trait to check if a type behaves like a complex number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
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


/**
 * @brief Concept for complex-like types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
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

/**
 * @brief Type trait to check if a type is an interval type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
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


/**
 * @brief Concept for interval-like types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
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
 * @brief Concept for all types accepted as series elements (FloatLike, ComplexLike, or IntervalLike).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template<typename T>
concept AcceptedLike = requires{ 
    requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; 
};

/**
 * @brief Concept for unsigned integral types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam K The type to check.
 */
template<typename K>
concept UnsignedIntLike = requires {
    std::is_integral<K>::value && !std::is_signed<K>::value;
};

/**
 * @brief Type trait to check if a type is an unsigned integral type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam K The type to check.
 */
template<typename K>
struct isUnsignedIntLike : std::integral_constant<bool, std::is_integral<K>::value && !std::is_signed<K>::value>{};

#endif