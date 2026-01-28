#ifndef UTILS_MATH_FWD_HPP
#define UTILS_MATH_FWD_HPP
#pragma once

template<typename T>
struct utils::helpers;

template<typename T> static T utils::helpers<T>::nextafter(const T& x, const T& y);

template<typename T> static T utils::helpers<T>::fmod(const T& x, const T& y);
template<typename T> static T utils::helpers<T>::floor(const T& x);
template<typename T> static T utils::helpers<T>::ceil(const T& x);

template<typename T> static T utils::helpers<T>::get_nan(const T& x){
    static_assert(std::false_type{}, "utils::get_nan not implemented for type");
}

template<typename T> static std::size_t utils::helpers<T>::get_precision(const T& x){return std::size_t{0};}

template<typename T> static std::string utils::helpers<T>::to_string(const T& x);

template<typename T> static bool utils::helpers<T>::isfinite(const T& x);
template<typename T> static bool utils::helpers<T>::isnan(const T& x);
template<typename T> static bool utils::helpers<T>::isinf(const T& x);

template<typename T> static T utils::helpers<T>::epsilon(const T& x);
template<typename T> static T utils::helpers<T>::numeric_max(size_t precision = size_t{0});

#endif