#ifndef UTILS_GET_PRECISION_H
#define UTILS_GET_PRECISION_H
#pragma once

#include "utils_base.hpp"

/**
 * @file utils_get_precision.hpp
 * @brief This file contains implementations for acquiring precision of various types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
/**
 * @brief Get the precision of a variable
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return size_t precision
 */
template<AcceptedLike T>
size_t utils::get_precision(const T& x){

	// Standard types have no arbitrary precision to report
	if constexpr (is_standard_types<T>::value){
		return static_cast<size_t>(0);
	}
	#ifdef __MPREAL_H__
	// Handling MPFR mpreal precision
	if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::bits2digits(x.get_prec());
	}
	#endif
	#ifdef INC_FPRECISION
	// Handling float_precision from custom libraries
	else if constexpr (std::is_same<T, float_precision>::value){
		return x.precision();
	}
	#ifdef INC_COMPLEXPRECISION
	// Complex types precision is the maximum of real and imaginary parts
	else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		return std::max(x.real().precision(), x.imag().precision());
	}
	#endif
	#ifdef INC_INTERVALPRECISION
	// Interval types precision is the maximum of left and right boundaries
	else if constexpr (std::is_same<T, interval<float_precision>>::value){
		return std::max(x.leftinterval().precision(), x.rightinterval().precision());
	}
	#endif
	#endif
	// Recursive handling for custom complex types
	if constexpr(is_complex_custom<T>::value) return std::max(utils::get_precision(x.real()), utils::get_precision(x.imag()));
	else return static_cast<size_t>(0);
}
#endif

#endif