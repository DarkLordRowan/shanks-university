#ifndef UTILS_CAST_MPREAL_HPP
#define UTILS_CAST_MPREAL_HPP
#pragma once

#ifdef __MPREAL_H__

template <typename From>
    requires std::floating_point<From> || std::integral<From>
struct utils::cast<mpfr::mpreal, From> {
    mpfr::mpreal operator()(const From& x, size_t precision = size_t{0});
};

template <typename From>
    requires std::floating_point<From> || std::integral<From>
mpfr::mpreal utils::cast<mpfr::mpreal, From>::operator()(const From& x, size_t precision) {
    if (precision > 0)
    return mpfr::mpreal(x, mpfr::digits2bits(precision));
    else 
    return mpfr::mpreal(x);
}

#endif

#endif