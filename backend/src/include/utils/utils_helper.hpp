#ifndef UTILS_HELPER_H
#define UTILS_HELPER_H
#pragma once

#include "utils_base.hpp"
#include <type_traits>
#include <cmath>

/**
 * @file utils_helper.hpp
 * @brief This file contains various helper utilities like string conversion and finiteness checks.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Converts variables of various types to std::string
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return std::string representation
 */
template<typename T>
std::string utils::to_string(const T& x){ 

    // Formatting based on type properties and library support
    if constexpr (std::is_floating_point<T>::value || std::is_integral<T>::value) return std::to_string(x);
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
        #define MAX_PRECISION_AVAILABLE -1
		return x.toString(MAX_PRECISION_AVAILABLE, 10);
	}
	#endif
    else if constexpr (is_complex_custom<T>::value) return "( " + utils::to_string(x.real()) + ", " + utils::to_string(x.imag()) + ")";
    else {
        static_assert(dependent_false<T>::value, "utils::to_string not implemented for this type");
    }
}

/**
 * @brief Checks if a variable represents a finite value
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return bool (is finite)
 */
template<typename T>
bool utils::isfinite(const T& x){
    // Using standard or library-specific checks for finiteness
    if constexpr (std::is_floating_point<T>::value) return std::isfinite(x);
    else if constexpr (is_complex_t<T>::value) return std::isfinite(x.real()) && std::isfinite(x.imag());
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::isfinite(x) && !mpfr::isnan(x) && !mpfr::isinf(x);
	}
	#endif
    else if constexpr (is_complex_custom<T>::value){
		return utils::isfinite(x.real()) && utils::isfinite(x.imag());
	}
    else {
        static_assert(dependent_false<T>::value, "utils::isfinite not implemented for this type");
    }
}

/**
 * @brief Returns the machine epsilon for type T
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (epsilon value)
 */
template<typename T>
T utils::epsilon(const T& x){
    // Selecting the epsilon calculation method for the given type
    if constexpr (is_standard_types<T>::value) return std::numeric_limits<T>::epsilon();
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::machine_epsilon(x);
	}
	#endif
    else {
        static_assert(dependent_false<T>::value, "utils::epsilon not implemented for this type");
    }
}

#endif