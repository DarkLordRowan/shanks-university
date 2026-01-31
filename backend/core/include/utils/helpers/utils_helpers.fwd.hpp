#ifndef UTILS_HELPERS_FWD_HPP
#define UTILS_HELPERS_FWD_HPP
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
    static_assert(std::false_type{}, "utils::helpers::nextafter not implemented for type");
}

template <typename T>
T utils::helpers<T>::fmod(const T& x, const T& y) {
    static_assert(std::false_type{}, "utils::helpers::fmod not implemented for type");
}
template <typename T>
T utils::helpers<T>::floor(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::floor not implemented for type");
}
template <typename T>
T utils::helpers<T>::ceil(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::ceil not implemented for type");
}

template <typename T>
T utils::helpers<T>::get_nan() {
    static_assert(std::false_type{}, "utils::helpers::get_nan not implemented for type");
}

template <typename T>
std::size_t utils::helpers<T>::get_precision(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::get_precision not implemented for type");
}

template <typename T>
std::string utils::helpers<T>::to_string(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::to_string not implemented for type");
}

template <typename T>
bool utils::helpers<T>::isfinite(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::isfinite not implemented for type");
}
template <typename T>
bool utils::helpers<T>::isnan(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::isnan not implemented for type");
}
template <typename T>
bool utils::helpers<T>::isinf(const T& x) {
    static_assert(std::false_type{}, "utils::helpers::isinf not implemented for type");
}

template <typename T>
T utils::helpers<T>::epsilon(std::size_t precision) {
    static_assert(std::false_type{}, "utils::helpers::epsilon not implemented for type");
}
template <typename T>
T utils::helpers<T>::numeric_max(std::size_t precision) {
    static_assert(std::false_type{}, "utils::helpers::numeric_max not implemented for type");
}

#include "interval/utils_helpers_interval.hpp"
#include "mpreal/utils_helpers_mpreal.hpp"
#include "std_complex/utils_helpers_std_complex.hpp"
#include "std_floating_point/utils_helpers_std_floating_point.hpp"
#include "std_integral/utils_helpers_std_integral.hpp"
#include "operation_counter/utils_helpers_op_counter.hpp"

#endif