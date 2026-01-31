#ifndef PRECISABLE_CONCEPTS_HPP
#define PRECISABLE_CONCEPTS_HPP
#pragma once

/**
 * @brief Type trait to check if a type supports explicit precision settings.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_precisable : public std::false_type {};

#ifdef __MPREAL_H__
template <> struct is_precisable<mpfr::mpreal> : public std::true_type {};
template <> struct is_precisable<std::complex<mpfr::mpreal>> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <> struct is_precisable<shanks::profiling::OperationCounting<mpfr::mpreal>> : public std::true_type {};
template <> struct is_precisable<std::complex<shanks::profiling::OperationCounting<mpfr::mpreal>>> : public std::true_type {};
#endif
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

#endif