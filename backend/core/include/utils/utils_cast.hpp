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
constexpr To utils::cast(const From& x, const size_t precision){

	// Default static cast for standard types
	if constexpr (is_standard_types<To>::value) return static_cast<To>(x);
	else if constexpr (is_precisable<To>::value)
		if (precision == size_t{0}) return static_cast<To>(x);
		else{
			if constexpr (std::is_same<To, mpfr::mpreal>::value) 
				if constexpr (std::is_same<From, mpfr::mpreal>::value) return mpfr::mpreal(x);
				else return mpfr::mpreal(x, mpfr::digits2bits(precision));
			else if constexpr(std::is_same<To, std::complex<mpfr::mpreal>>::value) return std::complex<mpfr::mpreal>(
				utils::cast<mpfr::mpreal>(x, precision),
				mpfr::mpreal(0.0, mpfr::digits2bits(precision))
			);
			else static_assert(dependent_false<To>::value, "utils::cast with precision not implemented for this type");
		}
	else  static_assert(dependent_false<To>::value, "utils::cast not implemented for this type");
	
}

#endif