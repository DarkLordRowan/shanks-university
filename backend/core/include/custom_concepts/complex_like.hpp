#ifndef COMPLEX_LIKE_HPP
#define COMPLEX_LIKE_HPP

/**
 * @brief Type trait to check if a type is a standard complex type with floating-point components.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */

template <typename T>
struct is_complex_t : public std::false_type {};

template <std::floating_point T>
struct is_complex_t<std::complex<T>> : public std::true_type {};

#ifdef SHANKS_ENABLE_PROFILING
template <std::floating_point T>
struct is_complex_t<std::complex<shanks::profiling::OperationCounting<T>>> : public std::true_type {};
#endif

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
#ifdef SHANKS_ENABLE_PROFILING
template <>
struct isComplexLike<shanks::profiling::OperationCounting<std::complex<mpfr::mpreal>>> : public std::true_type {};
#endif
#endif

template <typename T>
concept ComplexLike = isComplexLike<T>::value;

#endif