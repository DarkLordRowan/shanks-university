#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template <typename T>
    struct utils::helpers {
    static T nextafter(const T& x, const T& y);
    static T fmod(const T& x, const T& y);
    static T floor(const T& x);
    static T ceil(const T& x);
    static T get_nan();
    static std::size_t get_precision(const T& x);
    static std::string to_string(const T& x);
    static bool isfinite(const T& x);
    static bool isnan(const T& x);
    static bool isinf(const T& x);
    static T epsilon(std::size_t precision = std::size_t{0});
    static T numeric_max(std::size_t precision = std::size_t{0});
};

template <typename T>
T utils::helpers<T>::nextafter(const T& x, const T& y) {
    static_assert(std::false_type{}, "utils::nextafter not implemented for type");
}

template <typename T>
T utils::helpers<T>::fmod(const T& x, const T& y) {
    static_assert(std::false_type{}, "utils::fmod not implemented for type");
}
template <typename T>
T utils::helpers<T>::floor(const T& x) {
    static_assert(std::false_type{}, "utils::floor not implemented for type");
}
template <typename T>
T utils::helpers<T>::ceil(const T& x) {
    static_assert(std::false_type{}, "utils::ceil not implemented for type");
}

template <typename T>
T utils::helpers<T>::get_nan() {
    static_assert(std::false_type{}, "utils::get_nan not implemented for type");
}

template <typename T>
std::size_t utils::helpers<T>::get_precision(const T& x) {
    static_assert(std::false_type{}, "utils::get_precision not implemented for type");
}

template <typename T>
std::string utils::helpers<T>::to_string(const T& x) {
    static_assert(std::false_type{}, "utils::to_string not implemented for type");
}

template <typename T>
bool utils::helpers<T>::isfinite(const T& x) {
    static_assert(std::false_type{}, "utils::isfinite not implemented for type");
}
template <typename T>
bool utils::helpers<T>::isnan(const T& x) {
    static_assert(std::false_type{}, "utils::isnan not implemented for type");
}
template <typename T>
bool utils::helpers<T>::isinf(const T& x) {
    static_assert(std::false_type{}, "utils::isinf not implemented for type");
}

template <typename T>
T utils::helpers<T>::epsilon(std::size_t precision) {
    static_assert(std::false_type{}, "utils::epsilon not implemented for type");
}
template <typename T>
T utils::helpers<T>::numeric_max(std::size_t precision) {
    static_assert(std::false_type{}, "utils::numeric_max not implemented for type");
}

//#include "interval/utils_helpers_interval.hpp"
//#include "mpreal/utils_helpers_mpreal.hpp"
//#include "std_complex/utils_helpers_std_complex.hpp"
#include "std_floating_point/utils_helpers_std_floating_point.hpp"

#endif