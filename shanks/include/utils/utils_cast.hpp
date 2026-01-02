#ifndef UTILS_CAST_H
#define UTILS_CAST_H
#pragma once

#include "utils_base.hpp"

/**
 * @file utils_cast.hpp
 * @brief This file contains implementation of casting utilities.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Wrapper around std::static_cast with CLN specializations
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (From)
 * @return To (casted value)
 */
template<typename To, typename From >
constexpr To utils::cast(const From& x){

	// Special handling for CLN types to ensure proper conversion from integrals
	#ifdef _CL_FLOAT_CLASS_H
	if constexpr (std::is_same<To, cln::cl_F>::value && std::is_integral<From>::value) return static_cast<To>(static_cast<double>(x));
	else if constexpr (std::is_same<From, cln::cl_F>::value && std::is_integral<To>::value) return static_cast<To>(utils::cast<double>(x));
	else return static_cast<To>(x);
	#else
	// Default static cast for standard types
	return static_cast<To>(x);
	#endif
	
}

#endif