#ifndef UTILS_HELPER_STD_FLOATING_POINT_HPP
#define UTILS_HELPER_STD_FLOATING_POINT_HPP
#pragma once

template<std::floating_point T>
struct utils::helpers<T>;

template<std::floating_point T> constexpr static T utils::helpers<T>::nextafter(const T& x, const T& y){return std::nextafter(x,y);}

template<std::floating_point T> constexpr static T utils::helpers<T>::fmod(const T& x, const T& y){ return std::fmod(x,y);}
template<std::floating_point T> constexpr static T utils::helpers<T>::floor(const T& x){return std::floor(x);}
template<std::floating_point T> constexpr static T utils::helpers<T>::ceil(const T& x){return std::ceil(x);}

template<std::floating_point T> constexpr static T utils::helpers<T>::get_nan(const T& x){return std::nan("")}

template<std::floating_point T> constexpr static std::size_t utils::helpers<T>::get_precision(const T& x){return std::size_t{0};}

template<std::floating_point T> constexpr static std::string utils::helpers<T>::to_string(const T& x){return std::to_string(x);}

template<std::floating_point T> constexpr static bool utils::helpers<T>::isfinite(const T& x){return std::isfinite(x);}
template<std::floating_point T> constexpr static bool utils::helpers<T>::isnan(const T& x){return std::isnan(x);}
template<std::floating_point T> constexpr static bool utils::helpers<T>::isinf(const T& x){return std::isinf(x);}

template<std::floating_point T> constexpr static T utils::helpers<T>::epsilon(const T& x){return std::numeric_limits<T>::epsilon();}
template<std::floating_point T> constexpr static T utils::helpers<T>::numeric_max(size_t precision = size_t{0}){return std::numeric_limits<T>::max();}

#endif