#ifndef UNSIGNED_INT_LIKE_HPP
#define UNSIGNED_INT_LIKE_HPP
#pragma once

/**
 * @brief Type trait to check if a type is an unsigned integral type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct isUnsignedIntLike : public std::false_type {};

template <typename T>
    requires std::is_integral<T>::value && (!std::is_signed<T>::value)
struct isUnsignedIntLike<T> : public std::true_type {};

/**
 * @brief Concept for unsigned integral types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
concept UnsignedIntLike = isUnsignedIntLike<T>::value;

#endif