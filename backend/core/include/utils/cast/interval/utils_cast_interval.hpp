#ifndef UTILS_CAST_INTERVAL_HPP
#define UTILS_CAST_INTERVAL_HPP
#pragma once

template <typename To, typename From>
    requires std::floating_point<From> || std::integral<From>
struct utils::cast<intprec::interval<To>, From> {
    static intprec::interval<To> meta(const From& x, std::size_t precision = std::size_t{0});
    static intprec::interval<To> meta(const From& x, const From& y, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
    requires std::floating_point<From> || std::integral<From>
intprec::interval<To> utils::cast<intprec::interval<To>, From>::meta(const From& x, std::size_t precision) {
    return intprec::interval<To>(utis::cast<To>(x, precision));
}

template <typename To, typename From>
    requires std::floating_point<From> || std::integral<From>
intprec::interval<To> utils::cast<intprec::interval<To>, From>::meta(const From& x, const From& y, std::size_t precision) {
    return intprec::interval<To>(utis::cast<To>(x, precision), utis::meta<To>(y, precision));
}

#endif