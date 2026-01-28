#ifndef UTILS_GET_PRECISION_H
#define UTILS_GET_PRECISION_H
#pragma once

#include "utils_base.hpp"

/**
 * @file utils_get_precision.hpp
 * @brief This file contains implementations for acquiring precision of various types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Get the precision of a variable
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return size_t precision
 */
template <AcceptedLike T>
size_t utils::get_precision(const T& x) {
    // Standard types have no arbitrary precision to report
    if constexpr (is_standard_types<T>::value) {
        return static_cast<size_t>(0);
    }
#ifdef __MPREAL_H__
    // Handling MPFR mpreal precision
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) {
        return static_cast<size_t>(x.get_prec());
    }
#endif
    // Recursive handling for custom complex types
    else if constexpr (is_complex_custom<T>::value)
        return std::max(utils::get_precision(x.real()), utils::get_precision(x.imag()));
    else {
        if constexpr (requires { x.value; }) {
            return utils::get_precision(x.value);
        } else if constexpr (AcceptedLike<T>) {
            return size_t(0);
        } else {
            static_assert(dependent_false<T>::value, "utils::get_precision not implemented for this type");
        }
    }
}

#endif
