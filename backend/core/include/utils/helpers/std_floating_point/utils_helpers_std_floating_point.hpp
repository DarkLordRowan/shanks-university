#ifndef UTILS_HELPERS_STD_FLOATING_POINT_HPP
#define UTILS_HELPERS_STD_FLOATING_POINT_HPP
#pragma once

template <std::floating_point T>
struct utils::helpers<T> {
    static T nextafter(const T& x, const T& y);
    static T fmod(const T& x, const T& y);
    static T floor(const T& x);
    static T ceil(const T& x);
    static T get_nan();
    static T frexp(const T& x, int* exp);
    static std::size_t get_precision(const T& x);
    static std::string to_string(const T& x);
    static bool isfinite(const T& x);
    static bool isnan(const T& x);
    static bool isinf(const T& x);
    static T epsilon(std::size_t precision = std::size_t{0});
    static T numeric_max(std::size_t precision = std::size_t{0});
};

template <std::floating_point T>
T utils::helpers<T>::nextafter(const T& x, const T& y) {
    return std::nextafter(x, y);
}

template <std::floating_point T>
T utils::helpers<T>::fmod(const T& x, const T& y) {
    return std::fmod(x, y);
}
template <std::floating_point T>
T utils::helpers<T>::floor(const T& x) {
    return std::floor(x);
}
template <std::floating_point T>
T utils::helpers<T>::ceil(const T& x) {
    return std::ceil(x);
}

template <std::floating_point T>
T utils::helpers<T>::get_nan() {
    return std::nan("");
}

template <std::floating_point T>
T utils::helpers<T>::frexp(const T& x, int* exp){
    return std::frexp(x, exp);
}

template <std::floating_point T>
std::size_t utils::helpers<T>::get_precision(const T& x) {
    return std::size_t{0};
}

template <std::floating_point T>
std::string utils::helpers<T>::to_string(const T& x) {
    return std::to_string(x);
}

template <std::floating_point T>
bool utils::helpers<T>::isfinite(const T& x) {
    return std::isfinite(x);
}
template <std::floating_point T>
bool utils::helpers<T>::isnan(const T& x) {
    return std::isnan(x);
}
template <std::floating_point T>
bool utils::helpers<T>::isinf(const T& x) {
    return std::isinf(x);
}

template <std::floating_point T>
T utils::helpers<T>::epsilon(std::size_t precision) {
    return std::numeric_limits<T>::epsilon();
}
template <std::floating_point T>
T utils::helpers<T>::numeric_max(std::size_t precision) {
    return std::numeric_limits<T>::max();
}

#endif