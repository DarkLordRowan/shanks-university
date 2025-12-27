#ifndef UTILS_GET_PRECISION_H
#define UTILS_GET_PRECISION_H
#pragma once

#include "utils_base.hpp"

#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
template<AcceptedLike T>
size_t utils::get_precision(const T& x){

	if constexpr (is_standart_types<T>::value){
		return static_cast<size_t>(0);
	}
	#ifdef __MPREAL_H__
	if constexpr (std::is_same<T, mpfr::mpreal>::value){
		return mpfr::bits2digits(x.get_prec());
	}
	#endif
	#ifdef INC_FPRECISION
	else if constexpr (std::is_same<T, float_precision>::value){
		return x.precision();
	}
	#ifdef INC_COMPLEXPRECISION
	else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
		return std::max(x.real().precision(), x.imag().precision());
	}
	#endif
	#ifdef INC_INTERVALPRECISION
	else if constexpr (std::is_same<T, interval<float_precision>>::value){
		return std::max(x.leftinterval().precision(), x.rightinterval().precision());
	}
	#endif
	#endif
	else return static_cast<size_t>(0);
}
#endif

#endif