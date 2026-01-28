#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template <typename To, typename From>
struct utils::cast{
    /**
     * @brief Wrapper around std::static_cast, has similar functionality
     *
     * Used if custom type has limitations that does not allow usage of std::static_cast with no restrictions
     * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
     * @param x (From)
     * @return To (casted value)
    */
    static To cast(const From& x, size_t precision = size_t{0});
}

#endif