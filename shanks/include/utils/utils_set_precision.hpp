#ifndef UTILS_SET_PRECISION_H
#define UTILS_SET_PRECISION_H
#pragma once

#include "utils_base.hpp"

/**
 * @file utils_set_precision.hpp
 * @brief This file contains implementations for setting precision of various types.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

#if defined(INC_FPRECISION) || defined(__MPREAL_H__)
/**
 * @brief Sets precision of given variables for MPFR and custom types
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param precision (size_t), precisable_arg (Arg), precisable_args (Args)
 */
template<typename Arg, typename... Args>
requires (is_precisable<Args>::value && ...)
void utils::set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args){

	// Handling precision for various library-specific types
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
	// Recursive application for complex custom types
	if constexpr (is_complex_custom<Arg>::value){
		utils::set_precision(precision, reinterpret_cast<Arg::value_type(&)[2]>(precisable_arg)[0]);
		utils::set_precision(precision, reinterpret_cast<Arg::value_type(&)[2]>(precisable_arg)[1]);
	}
	// Processing the rest of the arguments
	utils::set_precision(precision, precisable_args...);
}
#endif

#ifdef SET_PRECISION_SET
/**
 * @brief Set the vector precision
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param vec (std::vector<T>), precision (size_t)
 */
template<AcceptedLike T>
void utils::set_vec_precision(std::vector<T>& vec, const size_t precision){
	// Applying precision setting to each element of the vector
	for(size_t j = 0; j < vec.size(); ++j)
		utils::set_precision(precision, vec[j]);
}
#endif

#endif