#ifndef UTILS_HELPER_STD_COMPLEX_HPP
#define UTILS_HELPER_STD_COMPLEX_HPP
#pragma once

template <typename T>
struct utils::helpers<std::complex<T>> {
    static std::complex<T> nextafter(const std::complex<T>& x, const std::complex<T>& y);
    static std::complex<T> fmod(const std::complex<T>& x, const std::complex<T>& y);
    static std::complex<T> floor(const std::complex<T>& x);
    static std::complex<T> ceil(const std::complex<T>& x);
    static std::complex<T> get_nan();
    static std::size_t get_precision(const std::complex<T>& x);
    static std::string to_string(const std::complex<T>& x);
    static bool isfinite(const std::complex<T>& x);
    static bool isnan(const std::complex<T>& x);
    static bool isinf(const std::complex<T>& x);
    static T epsilon(std::size_t precision = std::size_t{0});
    static T numeric_max(std::size_t precision = std::size_t{0});
};

template <typename T>
std::complex<T> utils::helpers<std::complex<T>>::nextafter(const std::complex<T>& x, const std::complex<T>& y) {
    static_assert(std::false_type{}, "utils::helpers::nextafter not implemented for type");
}

template <typename T>
std::complex<T> utils::helpers<std::complex<T>>::fmod(const std::complex<T>& x, const std::complex<T>& y) {
    static_assert(std::false_type{}, "utils::helpers::fmod not implemented for type");
}
template <typename T>
std::complex<T> utils::helpers<std::complex<T>>::floor(const std::complex<T>& x) {
    static_assert(std::false_type{}, "utils::helpers::floor not implemented for type");
}
template <typename T>
std::complex<T> utils::helpers<std::complex<T>>::ceil(const std::complex<T>& x) {
    static_assert(std::false_type{}, "utils::helpers::ceil not implemented for type");
}

template <typename T>
std::complex<T> utils::helpers<std::complex<T>>::get_nan() {
    return std::complex<T>(utils::helpers<T>::get_nan(),utils::helpers<T>::get_nan());
}

template <typename T>
std::size_t utils::helpers<std::complex<T>>::get_precision(const std::complex<T>& x) {
    return std::max(utils::helpers<T>::get_precision(x.real()), utils::helpers<T>::get_precision(x.imag()));
}

template <typename T>
std::string utils::helpers<std::complex<T>>::to_string(const std::complex<T>& x) {
    return "(" + utils::helpers<T>::to_string(x.real()) + ", " + utils::helpers<T>::to_string(x.imag()) + ")";
}

template <typename T>
bool utils::helpers<std::complex<T>>::isfinite(const std::complex<T>& x) {
    return utils::helpers<T>::isfinite(x.real()) && utils::helpers<T>::isfinite(x.imag());
}
template <typename T>
bool utils::helpers<std::complex<T>>::isnan(const std::complex<T>& x) {
    return utils::helpers<T>::isnan(x.real()) && utils::helpers<T>::isnan(x.imag());
}
template <typename T>
bool utils::helpers<std::complex<T>>::isinf(const std::complex<T>& x) {
    return utils::helpers<T>::isinf(x.real()) && utils::helpers<T>::isinf(x.imag());
}

template <typename T>
T utils::helpers<std::complex<T>>::epsilon(std::size_t precision) {
    return utils::helpers<T>::epsilon(precision);
}
template <typename T>
T utils::helpers<std::complex<T>>::numeric_max(std::size_t precision) {
    return utils::helpers<T>::numeric_max(precision);
}

#endif