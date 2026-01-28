#ifndef UTILS_CAST_MPREAL_HPP
#define UTILS_CAST_MPREAL_HPP
#pragma once

template <typename From>
    requires std::floating_point<From> || std::integral<From>
struct utils::cast<mpfr::mpreal, From> {
    mpfr::mpreal meta(const From& x, size_t precision = size_t{0});
};

template <typename From>
    requires std::floating_point<From> || std::integral<From>
mpfr::mpreal utils::cast<mpfr::mpreal, From>::meta(const From& x, size_t precision) {
    return mpfr::mpreal(x, precision);
}

#endif