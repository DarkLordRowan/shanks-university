#ifndef CUSTOM_CONCEPTS_HPP
#define CUSTOM_CONCEPTS_HPP
#pragma once

#include <complex>

// clang-format off
// Critical Section
#include "custom_types/mpreal.h"
#include "custom_concepts/float_like.hpp"

#include "custom_types/intervalprecision.fwd.hpp"
#include "custom_concepts/interval_like.hpp"

#include "custom_concepts/unsigned_int_like.hpp"
#include "custom_concepts/precisable_like.hpp"
#include "custom_concepts/complex_like.hpp"
// clang-format on

/**
 * @brief Type trait to check if a type is considered a "standard" project type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template <typename T>
struct is_standard_types : public std::false_type {};

template <typename T>
    requires std::numeric_limits<T>::is_integer
struct is_standard_types<T> : public std::true_type {};
template <typename T>
    requires std::numeric_limits<T>::is_integer
struct is_standard_types<std::complex<T>> : public std::true_type {};
template <std::floating_point T>
struct is_standard_types<T> : public std::true_type {};
template <std::floating_point T>
struct is_standard_types<std::complex<T>> : public std::true_type {};

template <typename T>
concept AcceptedLike = requires { requires FloatLike<T> || ComplexLike<T> || IntervalLike<T>; };

/**
 * @brief Utility for static_assert in if constexpr branches.
 */
//template <typename...>
//struct dependent_false : std::false_type {};

#endif