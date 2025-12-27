#ifndef UTILS_HELPER_H
#define UTILS_HELPER_H
#pragma once

#include "utils_base.hpp"
#include <type_traits>
#include <cmath>


template<typename T>
std::string utils::to_string(const T& x){ 

    if constexpr (is_standart_types<T>::value) return std::to_string(x);
    #ifdef INC_FPRECISION
    else if constexpr (std::is_same<T, float_precision>::value) return x.toString();
    #endif
    #ifdef INC_COMPLEXPRECISION
    else if constexpr (isComplexLike<T>::value) return utils::to_string(x.real()) + "i * " + utils::to_string(x.imag());
    #endif
    #ifdef INC_INTERVALPRECISION
    else if constexpr (is_interval<T>::value) return x.toString(); else
    #endif
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
        #define MAX_PRECISION_AVAILABLE -1
		return x.toString(MAX_PRECISION_AVAILABLE, 10);
	}
	#endif

    return "something went wrong";
}

template<typename T>
bool utils::isfinite(const T& x){
    if constexpr (is_standart_types<T>::value) return std::isfinite(x);
    #ifdef INC_FPRECISION
    else if constexpr (std::is_same<T, float_precision>::value) return isfinite(x);
    #endif
    #ifdef INC_COMPLEXPRECISION
    else if constexpr (isComplexLike<T>::value) return utils::isfinite(x.real()) && utils::isfinite(x.imag());;
    #endif
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::isfinite(x) && !mpfr::isnan(x) && !mpfr::isinf(x);
	}
	#endif
    
    return true;
}

template<typename T>
T utils::epsilon(const T& x){
    if constexpr (is_standart_types<T>::value) return std::numeric_limits<T>::epsilon();
    #ifdef INC_FPRECISION
    else if constexpr (std::is_same<T, float_precision>::value) return x.epsilon();
    #endif
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::machine_epsilon(x);
	}
	#endif
    
    return static_cast<T>(-1.0);
}

#endif