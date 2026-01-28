#ifndef UTILS_HELPER_STD_COMPLEX_HPP
#define UTILS_HELPER_STD_COMPLEX_HPP
#pragma once

#ifndef UTILS_HELPER_STD_FLOATING_POINT_HPP
#define UTILS_HELPER_STD_FLOATING_POINT_HPP
#pragma once

template<typename T>
struct utils::helpers<std::complex<T>>;

template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::nextafter(const T& x, const T& y){return std::nextafter(x,y);}

template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::fmod(const T& x, const T& y){ return std::fmod(x,y);}
template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::floor(const T& x){return std::floor(x);}
template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::ceil(const T& x){return std::ceil(x);}

template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::get_nan(const T& x){}

template<std::floating_point T> constexpr static std::size_t utils::helpers<std::complex<T>>::get_precision(const T& x){return std::size_t{0};}

template<std::floating_point T> constexpr static std::string utils::helpers<std::complex<T>>::to_string(const T& x){
    return "(" + utils::helpers<T>::to_string(x.real()) + ", " utils::helpers<T>::to_string(x.imag()) + ")";
}

template<std::floating_point T> constexpr static bool utils::helpers<std::complex<T>>::isfinite(const T& x){return utils::helpers<T>::isfinite(x.real()) && utils::helpers<T>::isfinite(x.imag())}
template<std::floating_point T> constexpr static bool utils::helpers<std::complex<T>>::isnan(const T& x){return utils::helpers<T>::isnan(x.real()) && utils::helpers<T>::isnan(x.imag())}
template<std::floating_point T> constexpr static bool utils::helpers<std::complex<T>>::isinf(const T& x){return utils::helpers<T>::isinf(x.real()) && utils::helpers<T>::isinf(x.imag())}

template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::epsilon(const T& x){return utils::helpers<T>::epsilon();}
template<std::floating_point T> constexpr static T utils::helpers<std::complex<T>>::numeric_max(size_t precision = size_t{0}){return utils::helpers<T>::numeric_max(precision);}

#endif

#endif