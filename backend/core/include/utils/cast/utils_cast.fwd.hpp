#ifndef UTILS_CAST_FWD_HPP
#define UTILS_CAST_FWD_HPP
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
    To operator()(const From& x, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
To utils::cast<To, From>::operator()(const From& x, [[maybe_unused]] std::size_t precision) {
    return static_cast<To>(x);
}

#include "fprecision/utils_cast_fprecision.hpp"
#include "mpreal/utils_cast_mpreal.hpp"
#include "operation_counter/utils_cast_op_counter.hpp"
#include "std_complex/utils_cast_std_complex.hpp"

#endif