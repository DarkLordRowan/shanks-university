#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template <typename To, typename From>
struct utils::cast {
    /**
     * @brief Wrapper around std::static_cast, has similar functionality
     *
     * Used if custom type has limitations that does not allow usage of std::static_cast with no restrictions
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (From)
     * @return To (casted value)
     */
    static To operator()(const From& x, size_t precision = size_t{0});
}

template <typename To, typename From>
TO utils::cast::operator()(const From& x, size_t precision){
    return static_cast<To>(x);
}

#include "std_complex/utils_cast_std_complex.hpp"
#include "mpreal/utils_cast_mpreal.hpp"

#endif