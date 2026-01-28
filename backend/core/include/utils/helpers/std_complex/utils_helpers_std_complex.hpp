#ifndef UTILS_HELPER_STD_COMPLEX_HPP
#define UTILS_HELPER_STD_COMPLEX_HPP
#pragma once

#ifndef UTILS_HELPER_STD_FLOATING_POINT_HPP
#define UTILS_HELPER_STD_FLOATING_POINT_HPP
#pragma once

template <typename T>
struct utils::helpers<std::complex<T>> {};

template <typename T>
static std::complex<T> utils::helpers<T>::nextafter(const std::complex<T>& x, const std::complex<T>& y) {
    static_assert(std::false_type{}, "utils::nextafter not implemented for type");
}

template <typename T>
static std::complex<T> utils::helpers<T>::fmod(const std::complex<T>& x, const std::complex<T>& y) {
    static_assert(std::false_type{}, "utils::fmod not implemented for type");
}
template <typename T>
static std::complex<T> utils::helpers<T>::floor(const std::complex<T>& x) {
    static_assert(std::false_type{}, "utils::floor not implemented for type");
}
template <typename T>
static std::complex<T> utils::helpers<T>::ceil(const std::complex<T>& x) {
    static_assert(std::false_type{}, "utils::ceil not implemented for type");
}

template <typename T>
static std::complex<T> utils::helpers<std::complex<T>>::get_nan() {
    return std::complex<T>(utils::helpers<T>::get_nan())
}

template <typename T>
static std::size_t utils::helpers<std::complex<T>>::get_precision(const std::complex<T>& x) {
    return std::max(utils::helpers<T>::get_precision(x.real()), utils::helpers<T>::get_precision(x.imag()))
}

template <typename T>
static std::string utils::helpers<std::complex<T>>::to_string(const std::complex<T>& x) {
    return "(" + utils::helpers<T>::to_string(x.real()) + ", " utils::helpers<T>::to_string(x.imag()) + ")";
}

template <typename T>
static bool utils::helpers<std::complex<T>>::isfinite(const std::complex<T>& x) {
    return utils::helpers<T>::isfinite(x.real()) && utils::helpers<T>::isfinite(x.imag())
}
template <typename T>
static bool utils::helpers<std::complex<T>>::isnan(const std::complex<T>& x) {
    return utils::helpers<T>::isnan(x.real()) && utils::helpers<T>::isnan(x.imag())
}
template <typename T>
static bool utils::helpers<std::complex<T>>::isinf(const std::complex<T>& x) {
    return utils::helpers<T>::isinf(x.real()) && utils::helpers<T>::isinf(x.imag())
}

template <typename T>
static T utils::helpers<std::complex<T>>::epsilon(std::size_t precision = std::size_t{0}) {
    return utils::helpers<T>::epsilon(precision);
}
template <typename T>
static T utils::helpers<std::complex<T>>::numeric_max(std::size_t precision = std::size_t{0}) {
    return utils::helpers<T>::numeric_max(precision);
}

#endif

#endif