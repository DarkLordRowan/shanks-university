#ifndef UTILS_HELPER_MPREAL_HPP
#define UTILS_HELPER_MPREAL_HPP
#pragma once

template<>
struct utils::helpers<mpfr::mpreal>;

template<mpfr::mpreal> constexpr static T utils::helpers<T>::nextafter(const T& x, const T& y){return mpfr::nextafter(x,y);}

template<mpfr::mpreal> constexpr static T utils::helpers<T>::fmod(const T& x, const T& y){ return mpfr::fmod(x,y);}
template<mpfr::mpreal> constexpr static T utils::helpers<T>::floor(const T& x){return mpfr::floor(x);}
template<mpfr::mpreal> constexpr static T utils::helpers<T>::ceil(const T& x){return mpfr::ceil(x);}

template<mpfr::mpreal> constexpr static T utils::helpers<T>::get_nan(const T& x){return mpfr::quiet_NaN();}

template<mpfr::mpreal> constexpr static std::size_t utils::helpers<T>::get_precision(const T& x){return mpfr::bits2digits(x.get_prec());}

template<mpfr::mpreal> constexpr static std::string utils::helpers<T>::to_string(const T& x){return x.toString(-1, 10);}

template<mpfr::mpreal> constexpr static bool utils::helpers<T>::isfinite(const T& x){return mpfr::isfinite(x);}
template<mpfr::mpreal> constexpr static bool utils::helpers<T>::isnan(const T& x){return mpfr::isnan(x);}
template<mpfr::mpreal> constexpr static bool utils::helpers<T>::isinf(const T& x){return mpfr::isinf(x);}

template<mpfr::mpreal> constexpr static T utils::helpers<T>::epsilon(const T& x){return mpfr::machine_epsilon(x);}
template<mpfr::mpreal> constexpr static T utils::helpers<T>::numeric_max(size_t precision = size_t{0}){return std::numeric_limits<mpfr::mpreal>::max(mpfr::digits2bits(precision));}

#endif