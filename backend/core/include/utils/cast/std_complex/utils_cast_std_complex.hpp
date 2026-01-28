#ifndef UTILS_CAST_STD_COMPLEX_HPP
#define UTILS_CAST_STD_COMPLEX_HPP
#pragma once

template <typename To, typename From>
struct utils::cast<std::complex<To>, From> {
    static std::complex<To> operator()(const From& x, size_t precision = size_t{0});
    static std::complex<To> operator()(const From& x, const From& y, size_t precision = size_t{0});
}

template <typename To, typename From>
TO utils::cast<std::complex<To>, From>::operator()(const From& x, size_t precision){
    return std::complex(utils::cast<To, From>(x, precision), utils::cast<To, From>(0, precision));
}

template <typename To, typename From>
struct utils::cast<std::complex<To>, std::complex<From>> {
    static std::complex<To> operator()(const std::complex<From>& x, size_t precision = size_t{0});
}

template <typename To, typename From>
std::complex<To> utils::cast<std::complex<To>, From>::operator()(const From& x, size_t precision){
    return std::complex(utils::cast<To, From>(x.real(), precision), utils::cast<To, From>(x.imag(), precision));
}

#endif