#ifndef UTILS_CAST_H
#define UTILS_CAST_H
#pragma once

#include "utils_base.hpp"

template<typename To, typename From >
To utils::cast(const From& x){

	#ifdef _CL_FLOAT_CLASS_H
	if constexpr (std::is_same<To, cln::cl_F>::value && std::is_integral<From>::value) return static_cast<To>(static_cast<double>(x));
	else if constexpr (std::is_same<From, cln::cl_F>::value && std::is_integral<To>::value) return static_cast<To>(utils::cast<double>(x));
	else return static_cast<To>(x);
	#else
	return static_cast<To>(x);
	#endif
	
}

#ifdef _CL_FLOAT_CLASS_H
template<>
inline double utils::cast(const cln::cl_F& x){
	return cln::double_approx(x);
}
template<>
inline float utils::cast(const cln::cl_F& x){
	return cln::float_approx(x);
}
#endif

#endif