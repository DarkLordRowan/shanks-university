#ifndef UTILS_MATH_H
#define UTILS_MATH_H
#pragma once

#include <type_traits>
#include <cmath>
#include <stdexcept>

#include "utils_base.hpp"

template<AcceptedLike T, UnsignedIntLike K>
constexpr const T utils::minus_one_raised_to_power_n(const K j){
    return (j & 1 ? utils::cast<T>(-1.0) : utils::cast<T>(1.0));
}

template <AcceptedLike T, UnsignedIntLike K>
constexpr const T utils::phi(K n)
{
	K result = n;
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}

	result -= n > 1 ? result / n : 0;
	return utils::cast<T>((result));
}

template<UnsignedIntLike K>
constexpr const K utils::fact(const K n) {
	K fact = static_cast<K>(1);
	for(K j = static_cast<K>(2); j <= n; ++j){
		fact *= j;
	}
	return fact;
}

template<UnsignedIntLike K>
constexpr const K utils::double_fact(const K n) {

	K double_fact = static_cast<K>(1);

	for (K j = n & static_cast<K>(1) + static_cast<K>(2); j <= n; j+=2){
		double_fact *= j;
	}

	return double_fact;
}

template<UnsignedIntLike K>
constexpr const K utils::binomial_coefficient(const K n, const K k) {

	if(n<k)
		throw std::invalid_argument("n>k");

	if (n==k || k==static_cast<K>(0))
		return static_cast<K>(1);

	const K new_k = (k > (n + n % 2) / 2 ? n - k : k);
	std::vector<K> dp(new_k + 1); dp[0] = 1;
	for(K i = 1; i <= n; ++i)
		for(K j = (i > new_k ? new_k : i); j > 0; --j)
			dp[j] += dp[j-1];
	
	return dp[new_k];
}

template<typename T> 
requires AcceptedLike<T> || std::is_integral<T>::value
T utils::fma(const T& a, const T& b, const T& c){
	if constexpr(is_standart_types<T>::value) return std::fma(a,b,c);
	#ifdef __MPREAL_H__
	else if constexpr(std::is_same<T, mpfr::mpreal>::value) return mpfr::fma(a,b,c);
	#endif
	else return a * b + c;
}

template<typename T> 
requires AcceptedLike<T> || std::is_integral<T>::value
T utils::pow(const T& x, const T& y){
	if constexpr (is_standart_types<T>::value) return std::pow(x,y);
	#ifdef _CL_FLOAT_CLASS_H
	else if constexpr (std::is_same<T, cln::cl_R>::value) return cln::exp(y * cln::ln(x));
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::pow(x,y);
	#endif
	else return static_cast<T>(0.0);
}

template<AcceptedLike T>
T utils::sqrt(const T& x){
	if (is_standart_types<T>::value) return std::sqrt(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sqrt(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sqrt(x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T>
T utils::exp(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::exp(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::exp(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return exp(x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T>
T utils::log(const T& x){
	if constexpr (is_standart_types<T>::value) return std::log(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::log(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return log(x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T> 
T utils::hypot(const T& a, const T& b){
	if constexpr(is_standart_types<T>::value) return std::hypot(a,b);
	#ifdef INC_FPRECISION
	if constexpr(std::is_same<T, float_precision>::value) return a.square() + b.square();
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::hypot(a, b);
	#endif
	else return sqrt(a*a + b*b);
}

template<AcceptedLike T> 
T utils::erf(const T& x){

	if constexpr (std::is_floating_point<T>::value) return std::erf(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::erf(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return erf(x);
	#endif
	else return utils::cast<T>(0.0);

}
template<AcceptedLike T>
T utils::zeta(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::riemann_zeta(x);
	#ifdef INC_FPRECISION
	else if constexpr (std::is_same<T, float_precision>::value) return abs(zeta(x));
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::zeta(x);
	#endif
	else return utils::cast<T>(0);
}

template<AcceptedLike T> 
T utils::ci_x(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) utils::cast<T>(gsl_sf_Ci(static_cast<double>(this->x)));
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::erf(x);
	#endif
	else return utils::cast<T>(0.0);
	#else 
	return utils::cast<T>(0.0);
	#endif
}

template<AcceptedLike T>
T utils::si_x(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return utils::cast<T>(gsl_sf_Si(static_cast<double>(this->x)));
	else return utils::cast<T>(0.0);
	#else 
	return utils::cast<T>(0.0);
	#endif
}

template<AcceptedLike T>
T utils::e_x(const T& x){
	if constexpr (std::is_floating_point<T>::value) std::comp_ellint_2(x);
	else return utils::cast<T>(0.0);
}


template<AcceptedLike T> 
T utils::k_x(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::comp_ellint_1(x);
	else return utils::cast<T>(0);
}

template<AcceptedLike T>
T utils::inc_gamma(const T& x, const T& alpha){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return std::tgamma(this->alpha) - utils::cast<T>(gsl_sf_gamma_inc(static_cast<double>(alpha), static_cast<double>(this->x)));
	#else 
	if constexpr (is_standart_types<T>::value) return utils::cast<T>(0.0);
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::tgamma(alpha) - mpfr::gammainc(alpha,x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T>
T utils::lambertW0(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return utils::cast<T>(gsl_sf_lambert_W0(static_cast<double>(this->x)));
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return lambertW0(x);
	#endif
	else return utils::cast<T>(0.0);
	#else 
	return utils::cast<T>(0.0);
	#endif
}

template<AcceptedLike T>
T utils::sin(const T& x){
	if constexpr (is_standart_types<T>::value) return std::sin(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sin(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sin(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T> 
T utils::asin(const T& x){
	if constexpr (is_standart_types<T>::value) return std::asin(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::asin(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return asin(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::cos(const T& x){
	if constexpr (is_standart_types<T>::value) return std::cos(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::cos(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return cos(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::acos(const T& x){
	if constexpr (is_standart_types<T>::value) return std::acos(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::acos(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return acos(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::tan(const T& x){
	if constexpr (is_standart_types<T>::value) return std::tan(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::tan(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return tan(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::atan(const T& x){
	if constexpr (is_standart_types<T>::value) return std::atan(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::atan(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return atan(x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T>
T utils::sinh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::sinh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sinh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sinh(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::asinh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::asinh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::asinh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return asinh(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::cosh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::cosh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::cosh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return cosh(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::acosh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::acosh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::acosh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return acosh(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::tanh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::tanh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::tanh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return tanh(x);
	#endif
	else return utils::cast<T>(0.0);
}
template<AcceptedLike T>
T utils::atanh(const T& x){
	if constexpr (is_standart_types<T>::value) return std::atanh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::atanh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return atanh(x);
	#endif
	else return utils::cast<T>(0.0);
}

template<AcceptedLike T>
typename GetUnderlyingType<T>::value utils::abs(const T& x){
	if constexpr (is_standart_types<T>::value) return std::abs(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::abs(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return abs(x);
	#endif
	else return utils::cast<typename GetUnderlyingType<T>::value>(0.0);
}

#endif