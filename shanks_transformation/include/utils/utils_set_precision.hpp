#ifndef UTILS_SET_PRECISION_H
#define UTILS_SET_PRECISION_H
#pragma once

#include "utils_base.hpp"

#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
template<typename Arg, typename... Args>
requires (is_precisable<Args>::value && ...)
void utils::set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args){

	
	#ifdef INC_FPRECISION
	if constexpr (std::is_same<Arg, float_precision>::value){
		precisable_arg.precision(precision);
	}
	#ifdef INC_COMPLEXPRECISION
	if constexpr (std::is_same<Arg, complex_precision<float_precision>>::value){
		precisable_arg.ref_real()->precision(precision); precisable_arg.ref_imag()->precision(precision);
	}
	#endif
	#ifdef INC_INTERVALPRECISION
	if constexpr (std::is_same<Arg, interval<float_precision>>::value){
		precisable_arg.ref_left()->precision(precision); precisable_arg.ref_right()->precision(precision);
	}
	#endif
	#endif
	#ifdef __MPREAL_H__
	if constexpr (std::is_same<Arg, mpfr::mpreal>::value){
		precisable_arg.set_prec(mpfr::digits2bits(precision));
	}
	#endif
	utils::set_precision(precision, precisable_args...);
}
#endif

#ifdef SET_PRECISION_SET
template<AcceptedLike T>
void utils::set_vec_precision(std::vector<T>& vec, const auto precision){
	for(size_t j = 0; j < vec.size(); ++j)
		utils::set_precision(precision, vec[j]);
}
#endif

#endif