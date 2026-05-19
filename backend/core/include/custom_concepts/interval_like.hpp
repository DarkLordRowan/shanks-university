#ifndef INTERVAL_LIKE_HPP
#define INTERVAL_LIKE_HPP
#pragma once

/**
 * @brief Type trait to check if a type is an interval type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_interval : public std::false_type {};

template <FloatLike T>
struct is_interval<intprec::interval<T>> : public std::true_type {};

template <typename T>
concept IntervalLike = is_interval<T>::value;

template <typename T>
inline constexpr bool is_interval_v = is_interval<T>::value;

template <typename T>
struct is_complex_interval : public std::false_type {};

template <FloatLike T>
struct is_complex_interval<std::complex<intprec::interval<T>>> : public std::true_type {};

template <typename T>
inline constexpr bool is_complex_interval_v = is_complex_interval<T>::value;

template <typename T>
struct interval_subtype {
    using value = T;
};

template <typename T>
struct interval_subtype<intprec::interval<T>> {
    using value = T;
};

#endif