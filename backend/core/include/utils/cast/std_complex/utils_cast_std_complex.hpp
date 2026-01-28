#ifndef UTILS_CAST_STD_COMPLEX_HPP
#define UTILS_CAST_STD_COMPLEX_HPP
#pragma once

template <typename To, typename From>
struct utils::cast<std::complex<To>, From> {
    static std::complex<To> meta(const From& x, std::size_t precision = std::size_t{0});
    static std::complex<To> meta(const From& x, const From& y, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
std::complex<To> utils::cast<std::complex<To>, From>::meta(const From& x, std::size_t precision) {
    return std::complex(utils::cast<To, From>::meta(x, precision), utils::cast<To, From>::meta(0, precision));
}

template <typename To, typename From>
struct utils::cast<std::complex<To>, std::complex<From>> {
    static std::complex<To> meta(const std::complex<From>& x, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
std::complex<To> utils::cast<std::complex<To>, std::complex<From>>::meta(const std::complex<From>& x,
                                                                               std::size_t precision) {
    return std::complex(utils::cast<To, From>::meta(x.real(), precision), utils::cast<To, From>::meta(x.imag(), precision));
}

#endif