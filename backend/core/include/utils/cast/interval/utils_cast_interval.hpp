#ifndef UTILS_CAST_INTERVAL_HPP
#define UTILS_CAST_INTERVAL_HPP
#pragma once

#ifdef INTERVALPRECISION_FWD_HPP

template <typename To, typename From>
struct utils::cast<intprec::interval<To>, From> {
    intprec::interval<To> operator()(const From& x, std::size_t precision = std::size_t{0});
};

template <typename To, typename From>
intprec::interval<To> utils::cast<intprec::interval<To>, From>::operator()(const From& x, std::size_t precision) {
    return intprec::interval<To>(utis::cast<To, From>(x, precision));
}

template <FloatLike To, typename From>
struct utils::cast<To, intprec::interval<From>> {
    To operator()(const intprec::interval<From>& x, std::size_t precision = std::size_t{0});
};

template <FloatLike To, typename From>
To utils::cast<To, intprec::interval<From>>::operator()(const intprec::interval<From>& x, std::size_t precision) {
    return utils::cast<To, From>(x.mid(), precision);
}

#endif

#endif