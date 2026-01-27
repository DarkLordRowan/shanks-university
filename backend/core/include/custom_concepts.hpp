#ifndef CUSTOM_CONCEPTS_HPP
#define CUSTOM_CONCEPTS_HPP
#pragma once

/**
 * @file custom_concepts.hpp
 * @brief This file contains the definitions of custom type traits and concepts used throughout the project.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#ifndef __MPREAL_H__
#include "mpreal.h"
#endif

#include <type_traits>

/**
 * @brief Type trait to check if a type behaves like a floating-point number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to inspect.
 */

template <typename T>
struct isFloatLike : public std::false_type {};

template <std::floating_point T>
struct isFloatLike<T> : public std::true_type {};
#ifdef __MPREAL_H__
template <>
struct isFloatLike<mpfr::mpreal> : public std::true_type {};
#endif

/**
 * @brief Concept for types that behave like floating-point numbers.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept FloatLike = isFloatLike<T>::value;

/**
 * @brief Type trait to check if a type is a standard complex type with floating-point components.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_complex_t : public std::false_type {};

template <std::floating_point U>
struct is_complex_t<std::complex<U>> : public std::true_type {};

/**
 * @brief Type trait to check if a type supports explicit precision settings.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_precisable : public std::false_type {};
#ifdef __MPREAL_H__
template <>
struct is_precisable<mpfr::mpreal> : public std::true_type {};
template <>
struct is_precisable<std::complex<mpfr::mpreal>> : public std::true_type {};
#endif

/**
 * @brief Type trait to check if a type is complex with components satisfying is_precisable.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_complex_custom : public std::false_type {};

template <typename U>
    requires is_precisable<U>::value
struct is_complex_custom<std::complex<U>> : public std::true_type {};

/**
 * @brief Type trait to check if a type is considered a "standard" project type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_standard_types : public std::false_type {};
template <std::floating_point T>
struct is_standard_types<T> : public std::true_type {};
template <typename T>
    requires is_complex_t<T>::value
struct is_standard_types<T> : public std::true_type {};
template <typename T>
    requires std::is_integral<T>::value
struct is_standard_types<T> : public std::true_type {};

/**
 * @brief Type trait to check if a type behaves like a complex number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct isComplexLike : public std::false_type {};
template <typename T>
    requires is_complex_t<T>::value
struct isComplexLike<T> : public std::true_type {};
#ifdef __MPREAL_H__
template <>
struct isComplexLike<std::complex<mpfr::mpreal>> : public std::true_type {};
#endif

/**
 * @brief Concept for complex-like types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept ComplexLike = isComplexLike<T>::value;

/**
 * @brief Type trait to check if a type is an interval type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_interval : std::integral_constant<bool, false> {};

/**
 * @brief Concept for interval-like types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept IntervalLike = false;

/**
 * @brief Concept for all types accepted as series elements (FloatLike, ComplexLike, or IntervalLike).
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept AcceptedLike = requires { requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; };

/**
 * @brief Concept for unsigned integral types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam K The type to check.
 */
template <typename K>
concept UnsignedIntLike = requires { std::is_integral<K>::value && !std::is_signed<K>::value; };

/**
 * @brief Type trait to check if a type is an unsigned integral type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam K The type to check.
 */
template <typename K>
struct isUnsignedIntLike : std::integral_constant<bool, std::is_integral<K>::value && !std::is_signed<K>::value> {};

/**
 * @brief Utility for static_assert in if constexpr branches.
 */
template <typename...>
struct dependent_false : std::false_type {};

#endif