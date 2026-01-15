#ifndef UTILS_MATH_H
#define UTILS_MATH_H
#pragma once

#include <type_traits>
#include <cmath>
#include <stdexcept>

#include "utils_base.hpp"

/**
 * @file utils_math.hpp
 * @brief This file contains implementations of various mathematical functions.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

/**
 * @brief Returns (-1)^j as type T
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param j (K)
 * @return T (-1 or 1)
 */
template<AcceptedLike T, UnsignedIntLike K>
constexpr T utils::minus_one_raised_to_power_n(const K j){ return (j & 1 ? utils::cast<T>(-1.0) : utils::cast<T>(1.0));}

/**
 * @brief Calculates Euler's totient function phi(n)
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param n (K)
 * @return T (phi result)
 */
template <AcceptedLike T, UnsignedIntLike K>
constexpr T utils::phi(K n)
{
	K result = n;
	// Iterating to find prime factors and applying the formula
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0) n /= i;
			result -= result / i;
		}

	// Final step for the remaining prime factor
	result -= n > 1 ? result / n : 0;
	return utils::cast<T>((result));
}

/**
 * @brief Calculates factorial of n
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param n (K)
 * @return K (factorial)
 */
template<UnsignedIntLike K>
constexpr K utils::fact(const K n) {
	K fact = static_cast<K>(1);
	for(K j = static_cast<K>(2); j <= n; ++j){
		fact *= j;
	}
	return fact;
}

/**
 * @brief Calculates double factorial n!!
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param n (K)
 * @return K (double factorial)
 */
template<UnsignedIntLike K>
constexpr K utils::double_fact(const K n) {

	K double_fact = static_cast<K>(1);

	// Multiplies every second integer down to 1 or 2
	for (K j = n & static_cast<K>(1) + static_cast<K>(2); j <= n; j+=2){
		double_fact *= j;
	}

	return double_fact;
}

/**
 * @brief Calculates binomial coefficient (n, k)
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param n (K), k (K)
 * @return K (coefficient)
 * @throws std::invalid_argument if n < k
 */
template<UnsignedIntLike K>
constexpr K utils::binomial_coefficient(const K n, const K k) {

	if(n<k)
		throw std::invalid_argument("n>k");

	if (n==k || k==static_cast<K>(0))
		return static_cast<K>(1);

	// Using DP approach for stability and avoiding large intermediate values
	const K new_k = (k > (n + n % 2) / 2 ? n - k : k);
	std::vector<K> dp(new_k + 1); dp[0] = 1;
	for(K i = 1; i <= n; ++i)
		for(K j = (i > new_k ? new_k : i); j > 0; --j)
			dp[j] += dp[j-1];
	
	return dp[new_k];
}

/**
 * @brief Fused multiply-add implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param a (T), b (T), c (T)
 * @return T (result)
 */
template<typename T> 
requires AcceptedLike<T> || std::is_integral<T>::value
T utils::fma(const T& a, const T& b, const T& c){
	if constexpr(std::is_floating_point<T>::value) return std::fma(a,b,c);
	#ifdef __MPREAL_H__
	else if constexpr(std::is_same<T, mpfr::mpreal>::value) return mpfr::fma(a,b,c);
	#endif
	else return a * b + c;
}

/**
 * @brief Power function implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T), y (T)
 * @return T (result)
 */
/**
 * @brief Power function implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T), y (T)
 * @return T (result)
 */
template<typename T> 
requires AcceptedLike<T> || std::is_integral<T>::value
T utils::pow(const T& x, const T& y){
	if constexpr (is_standard_types<T>::value) return std::pow(x,y);
	#ifdef _CL_FLOAT_CLASS_H
	else if constexpr (std::is_same<T, cln::cl_R>::value) return cln::exp(y * cln::ln(x));
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::pow(x,y);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        return utils::exp(y * utils::log(x));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::pow not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::pow not implemented for this type");
        }
    }
}

/**
 * @brief atan2 implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */
template<typename T>
T utils::atan2(const T& y, const T& x){
    if constexpr (std::is_floating_point<T>::value) return std::atan2(y, x);
    #ifdef __MPREAL_H__
    else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::atan2(y, x);
    #endif
    else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::atan2 not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::atan2 not implemented for this type");
        }
    }
}

/**
 * @brief Square root implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::sqrt(const T& x){
	if constexpr (is_standard_types<T>::value) return std::sqrt(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sqrt(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sqrt(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        using ComponentT = typename T::value_type;
        ComponentT r = utils::abs(x);
        ComponentT phi = utils::atan2(x.imag(), x.real());
        ComponentT sqrt_r = utils::sqrt(r);
        return T(sqrt_r * utils::cos(phi / 2.0), sqrt_r * utils::sin(phi / 2.0));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::sqrt not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::sqrt not implemented for this type");
        }
    }
}

/**
 * @brief Exponent implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::exp(const T& x){
	if constexpr (is_standard_types<T>::value) return std::exp(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::exp(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return exp(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        using ComponentT = typename T::value_type;
        ComponentT exp_r = utils::exp(x.real());
        return T(exp_r * utils::cos(x.imag()), exp_r * utils::sin(x.imag()));
    }
	else {
        static_assert(dependent_false<T>::value, "utils::exp not implemented for this type");
    }
}

/**
 * @brief Natural logarithm implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::log(const T& x){
	if constexpr (is_standard_types<T>::value) return std::log(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::log(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return log(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        using ComponentT = typename T::value_type;
        return T(utils::log(utils::abs(x)), utils::atan2(x.imag(), x.real()));
    }
	else {
        static_assert(dependent_false<T>::value, "utils::log not implemented for this type");
    }
}

/**
 * @brief Hypotenuse implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param a (T), b (T)
 * @return T (result)
 */
template<AcceptedLike T> 
T utils::hypot(const T& a, const T& b){
	if constexpr(is_standard_types<T>::value) return std::hypot(a,b);
	#ifdef INC_FPRECISION
	if constexpr(std::is_same<T, float_precision>::value) return a.square() + b.square();
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::hypot(a, b);
	#endif
	else return sqrt(a*a + b*b);
}

/**
 * @brief Error function implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T> 
T utils::erf(const T& x){

	if constexpr (std::is_floating_point<T>::value) return std::erf(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::erf(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return erf(x);
	#endif
	else {
        // If we can't implement it, throw at runtime for AcceptedLike types to allow compilation
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::erf not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::erf not implemented for this type");
        }
    }

}

/**
 * @brief Riemann zeta function implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::zeta(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::riemann_zeta(x);
	#ifdef INC_FPRECISION
	else if constexpr (std::is_same<T, float_precision>::value) return abs(zeta(x));
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::zeta(x);
	#endif
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::zeta not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::zeta not implemented for this type");
        }
    }
}

/**
 * @brief Trigonometric integral Ci(x) implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T> 
T utils::ci_x(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) utils::cast<T>(gsl_sf_Ci(static_cast<double>(this->x)));
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::erf(x);
	#endif
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::ci_x not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::ci_x not implemented for this type");
        }
    }
	#else 
	{
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::ci_x not implemented (GSL missing)");
        } else {
            static_assert(dependent_false<T>::value, "utils::ci_x not implemented (GSL missing)");
        }
    }
	#endif
}

/**
 * @brief Trigonometric integral Si(x) implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::si_x(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return utils::cast<T>(gsl_sf_Si(static_cast<double>(this->x)));
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::si_x not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::si_x not implemented for this type");
        }
    }
	#else 
	{
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::si_x not implemented (GSL missing)");
        } else {
            static_assert(dependent_false<T>::value, "utils::si_x not implemented (GSL missing)");
        }
    }
	#endif
}

/**
 * @brief Complete elliptical integral of the second kind implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::e_x(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::comp_ellint_2(x);
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::e_x not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::e_x not implemented for this type");
        }
    }
}

/**
 * @brief Complete elliptical integral of the first kind implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T> 
T utils::k_x(const T& x){
	if constexpr (std::is_floating_point<T>::value) return std::comp_ellint_1(x);
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::k_x not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::k_x not implemented for this type");
        }
    }
}

/**
 * @brief Lower incomplete gamma function implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T), alpha (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::inc_gamma(const T& x, const T& alpha){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return std::tgamma(this->alpha) - utils::cast<T>(gsl_sf_gamma_inc(static_cast<double>(alpha), static_cast<double>(this->x)));
	#else 
	if constexpr (is_standard_types<T>::value) {
        throw std::runtime_error("utils::inc_gamma not implemented (GSL missing)");
    }
	#endif
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::gammainc(alpha,x);
	#endif
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::inc_gamma not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::inc_gamma not implemented for this type");
        }
    }
}

/**
 * @brief Lambert's function pricipal branch (W_{0}) implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::lambertW0(const T& x){
	#ifdef __GSL_SF_EXPINT_H__
	if constexpr (std::is_floating_point<T>::value) return utils::cast<T>(gsl_sf_lambert_W0(static_cast<double>(this->x)));
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return lambertW0(x);
	#endif
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::lambertW0 not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::lambertW0 not implemented for this type");
        }
    }
	#else 
	{
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::lambertW0 not implemented (GSL missing)");
        } else {
            static_assert(dependent_false<T>::value, "utils::lambertW0 not implemented (GSL missing)");
        }
    }
	#endif
}

/**
 * @brief Sine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::sin(const T& x){
	if constexpr (is_standard_types<T>::value) return std::sin(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sin(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sin(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        using ComponentT = typename T::value_type;
        return T(utils::sin(x.real()) * utils::cosh(x.imag()), utils::cos(x.real()) * utils::sinh(x.imag()));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::sin not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::sin not implemented for this type");
        }
    }
}

/**
 * @brief Arcsine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T> 
T utils::asin(const T& x){
	if constexpr (is_standard_types<T>::value) return std::asin(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::asin(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return asin(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // asin(z) = -i * log(i*z + sqrt(1 - z^2))
        T i(0, 1);
        return -i * utils::log(i * x + utils::sqrt(utils::cast<T>(1.0) - x * x));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::asin not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::asin not implemented for this type");
        }
    }
}

/**
 * @brief Cosine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::cos(const T& x){
	if constexpr (is_standard_types<T>::value) return std::cos(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::cos(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return cos(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        using ComponentT = typename T::value_type;
        return T(utils::cos(x.real()) * utils::cosh(x.imag()), -utils::sin(x.real()) * utils::sinh(x.imag()));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::cos not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::cos not implemented for this type");
        }
    }
}

/**
 * @brief Arccosine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::acos(const T& x){
	if constexpr (is_standard_types<T>::value) return std::acos(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::acos(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return acos(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // acos(z) = pi/2 - asin(z)
        T i(0, 1);
        return -i * utils::log(x + i * utils::sqrt(utils::cast<T>(1.0) - x * x));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::acos not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::acos not implemented for this type");
        }
    }
}

/**
 * @brief Tangent implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::tan(const T& x){
	if constexpr (is_standard_types<T>::value) return std::tan(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::tan(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return tan(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        return utils::sin(x) / utils::cos(x);
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::tan not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::tan not implemented for this type");
        }
    }
}

/**
 * @brief Arctangent implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::atan(const T& x){
	if constexpr (is_standard_types<T>::value) return std::atan(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::atan(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return atan(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // atan(z) = (i/2) * (log(1 - i*z) - log(1 + i*z))
        T i(0, 1);
        T i2(0, 0.5);
        return i2 * (utils::log(utils::cast<T>(1.0) - i * x) - utils::log(utils::cast<T>(1.0) + i * x));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::atan not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::atan not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic sine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::sinh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::sinh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::sinh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return sinh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // sinh(z) = (exp(z) - exp(-z)) / 2
        return (utils::exp(x) - utils::exp(-x)) / utils::cast<T>(2.0);
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::sinh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::sinh not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic arcsine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::asinh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::asinh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::asinh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return asinh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // asinh(z) = log(z + sqrt(z^2 + 1))
        return utils::log(x + utils::sqrt(x * x + utils::cast<T>(1.0)));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::asinh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::asinh not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic cosine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::cosh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::cosh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::cosh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return cosh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // cosh(z) = (exp(z) + exp(-z)) / 2
        return (utils::exp(x) + utils::exp(-x)) / utils::cast<T>(2.0);
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::cosh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::cosh not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic arccosine implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::acosh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::acosh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::acosh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return acosh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // acosh(z) = log(z + sqrt(z + 1) * sqrt(z - 1))
        return utils::log(x + utils::sqrt(x + utils::cast<T>(1.0)) * utils::sqrt(x - utils::cast<T>(1.0)));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::acosh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::acosh not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic tangent implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::tanh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::tanh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::tanh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return tanh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        return utils::sinh(x) / utils::cosh(x);
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::tanh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::tanh not implemented for this type");
        }
    }
}

/**
 * @brief Hyperbolic arctangent implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return T (result)
 */
template<AcceptedLike T>
T utils::atanh(const T& x){
	if constexpr (is_standard_types<T>::value) return std::atanh(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::atanh(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return atanh(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        // atanh(z) = 0.5 * (log(1 + z) - log(1 - z))
        return utils::cast<T>(0.5) * (utils::log(utils::cast<T>(1.0) + x) - utils::log(utils::cast<T>(1.0) - x));
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::atanh not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::atanh not implemented for this type");
        }
    }
}

/**
 * @brief Absolute value implementation
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @param x (T)
 * @return Underlying type value (result)
 */
template<AcceptedLike T>
typename GetUnderlyingType<T>::value utils::abs(const T& x){
	if constexpr (is_standard_types<T>::value) return std::abs(x);
	#ifdef __MPREAL_H__
	else if constexpr (std::is_same<T, mpfr::mpreal>::value) return mpfr::abs(x);
	#endif
	#ifdef INC_FPRECISION
	else if constexpr(std::is_same<T, float_precision>::value) return abs(x);
	#endif
    else if constexpr (is_complex_custom<T>::value) {
        return mpfr::hypot(x.real(), x.imag());
    }
	else {
        if constexpr (AcceptedLike<T>) {
            throw std::runtime_error("utils::abs not implemented for this type");
        } else {
            static_assert(dependent_false<T>::value, "utils::abs not implemented for this type");
        }
    }
}

#endif