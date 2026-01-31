#ifndef FLOAT_LIKE_HPP
#define FLOAT_LIKE_HPP

/**
 * @brief Type trait to check if a type behaves like a floating-point number.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to inspect.
 */
template <typename T>
struct isFloatLike : public std::false_type {};

/// FOR std:: types
template <std::floating_point T>
struct isFloatLike<T> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template <std::floating_point T>
struct isFloatLike<shanks::profiling::OperationCounting<T>> : public std::true_type{};
#endif

/// FOR mpfr::mpreal
#ifdef __MPREAL_H__
template <>
struct isFloatLike<mpfr::mpreal> : public std::true_type {};
#ifdef SHANKS_ENABLE_PROFILING
template<>
struct isFloatLike<shanks::profiling::OperationCounting<mpfr::mpreal>> : public std::true_type{};
#endif
#endif

template <typename T>
concept FloatLike = isFloatLike<T>::value;

#endif