#ifndef UTILS_HELPER_INTERVAL_HPP
#define UTILS_HELPER_INTERVAL_HPP
#pragma once

template <typename T>
struct utils::helpers<intprec::interval<T>> {};

template <typename T>
static intprec::interval<T> utils::helpers<intprec::interval<T>>::nextafter(const intprec::interval<T>& x,
                                                                            const intprec::interval<T>& y) {
    static_assert(std::false_type{}, "utils::helpers::nextafter not implemented for type");
}

template <typename T>
static intprec::interval<T> utils::helpers<intprec::interval<T>>::fmod(const intprec::interval<T>& x,
                                                                       const intprec::interval<T>& y) {
    static_assert(std::false_type{}, "utils::helpers::fmod not implemented for type");
}
template <typename T>
static intprec::interval<T> utils::helpers<intprec::interval<T>>::floor(const intprec::interval<T>& x) {
    return intprec::floor(x);
}
template <typename T>
static intprec::interval<T> utils::helpers<intprec::interval<T>>::ceil(const intprec::interval<T>& x) {
    return intprec::ceil(x);
}

template <typename T>
static intprec::interval<T> utils::helpers<intprec::interval<T>>::get_nan() {
    return intprec::interval<T>(utils::helpers<T>::get_nan());
}

template <typename T>
static std::size_t utils::helpers<intprec::interval<T>>::get_precision(const intprec::interval<T>& x) {
    return std::max(utils::helpers<T>::get_precision(x.leftinterval()),
                    utils::helpers<T>::get_precision(x.rightinterval()));
}

template <typename T>
static std::string utils::helpers<intprec::interval<T>>::to_string(const intprec::interval<T>& x) {
    return x.toString();
}

template <typename T>
static bool utils::helpers<intprec::interval<T>>::isfinite(const intprec::interval<T>& x) {
    return utils::helpers<T>::isfinite(x.leftinterval()) && utils::helpers<T>::isfinite(x.rightinterval());
}
template <typename T>
static bool utils::helpers<intprec::interval<T>>::isnan(const intprec::interval<T>& x) {
    return utils::helpers<T>::isnan(x.leftinterval()) && utils::helpers<T>::isnan(x.rightinterval());
}
template <typename T>
static bool utils::helpers<intprec::interval<T>>::isinf(const intprec::interval<T>& x) {
    return utils::helpers<T>::isinf(x.leftinterval()) && utils::helpers<T>::isinf(x.rightinterval());
}

template <typename T>
static T utils::helpers<intprec::interval<T>>::epsilon(std::size_t precision = std::size_t{0}) {
    return utils::helpers<T>::epsilon(precision);
}
template <typename T>
static T utils::helpers<intprec::interval<T>>::numeric_max(std::size_t precision = std::size_t{0}) {
    return utils::helpers<T>::numeric_max(precision);
}

#endif