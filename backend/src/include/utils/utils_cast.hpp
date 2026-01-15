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

	// Default static cast for standard types
	return static_cast<To>(x);
	
}

#endif