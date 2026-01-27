#ifndef INTERVAL_LIKE_HPP
#define INTERVAL_LIKE_HPP
#pragma once

#include <type_traits>
#include "float_like.hpp"
#include "../custom_types/intervalprecision.fwd.hpp"

/**
 * @brief Type trait to check if a type is an interval type.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T The type to check.
 */
template<typename T>
struct is_interval : public std::false_type{};

template<FloatLike T>
struct is_interval<intprec::interval<T>> : public std::true_type{};

template<typename T>
concept IntervalLike = is_interval<T>::value;

#endif