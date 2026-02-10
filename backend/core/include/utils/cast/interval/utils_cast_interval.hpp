#ifndef UTILS_CAST_INTERVAL_HPP
#define UTILS_CAST_INTERVAL_HPP
#pragma once

#ifdef INTERVALPRECISION_FWD_HPP

template <typename To, typename From>
    requires std::floating_point<From> || std::integral<From>
struct utils::cast<intprec::interval<To>, From> {
    intprec::interval<To> operator()(const From& x, std::size_t precision = std::size_t{0});
    static intprec::interval<To> operator()(const From& x, const From& y, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
    requires std::floating_point<From> || std::integral<From>
intprec::interval<To> utils::cast<intprec::interval<To>, From>::operator()(const From& x, std::size_t precision) {
    return intprec::interval<To>(utis::cast<To, From>(x, precision));
}

// template <typename To, typename From>
//     requires std::floating_point<From> || std::integral<From>
// intprec::interval<To> utils::cast<intprec::interval<To>, From>::operator()(const From& x, const From& y,
//                                                                            std::size_t precision) {
//     return intprec::interval<To>(utis::cast<To, From>(x, precision), utis::cast<To, From>(y, precision));
// }

#endif

#endif