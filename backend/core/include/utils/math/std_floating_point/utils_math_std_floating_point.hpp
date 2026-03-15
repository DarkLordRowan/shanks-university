#ifndef UTILS_MATH_STD_FLOATING_POINT_HPP
#define UTILS_MATH_STD_FLOATING_POINT_HPP
#pragma once

template <std::floating_point T>
struct utils::math<T> {
    template <std::integral K>
    static T minus_one_raised_to_power_n(const K j);
    static T pow(const T& x, const T& y);
    static T atan2(const T& x, const T& y);
    static T fma(const T& a, const T& b, const T& c);
    static T sqrt(const T& x);
    static T exp(const T& x);
    static T log(const T& x);
    static T log10(const T& x);
    static T hypot(const T& a, const T& b);
    static T erf(const T& x);
    static T zeta(const T& x);
    static T e_x(const T& x);
    static T k_x(const T& x);
#ifdef __GSL_SF_EXPINT_H__
    static T ci_x(const T& x);
    static T si_x(const T& x);
    static T inc_gamma(const T& x, const T& alpha);
    static T lambertW0(const T& x);
#endif
    static T sin(const T& x);
    static T asin(const T& x);
    static T cos(const T& x);
    static T acos(const T& x);
    static T tan(const T& x);
    static T atan(const T& x);
    static T sinh(const T& x);
    static T asinh(const T& x);
    static T cosh(const T& x);
    static T acosh(const T& x);
    static T tanh(const T& x);
    static T atanh(const T& x);
    static T abs(const T& x);

    // availability of special function
    using has_erf = std::true_type;
    using has_zeta = std::true_type;
#ifdef __GSL_SF_EXPINT_H__
    using has_ci_x = std::true_type;
    using has_si_x = std::true_type;
    using has_inc_gamma = std::true_type;
    using has_lambertW0 = std::true_type;
#else
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
    using has_inc_gamma = std::false_type;
    using has_lambertW0 = std::false_type;
#endif
    using has_e_x = std::true_type;
    using has_k_x = std::true_type;
};

template <std::floating_point T>
template <std::integral K>
T utils::math<T>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return static_cast<T>(std::abs(j) & 1 ? -1 : 1);
    else
        return static_cast<T>(j & 1 ? -1 : 1);
}
template <std::floating_point T>
T utils::math<T>::pow(const T& x, const T& y) {
    return std::pow(x, y);
}
template <std::floating_point T>
T utils::math<T>::atan2(const T& x, const T& y) {
    return std::atan2(x, y);
}
template <std::floating_point T>
T utils::math<T>::fma(const T& a, const T& b, const T& c) {
    return std::fma(a, b, c);
}
template <std::floating_point T>
T utils::math<T>::sqrt(const T& x) {
    return std::sqrt(x);
}
template <std::floating_point T>
T utils::math<T>::exp(const T& x) {
    return std::exp(x);
}
template <std::floating_point T>
T utils::math<T>::log(const T& x) {
    return std::log(x);
}
template <std::floating_point T>
T utils::math<T>::log10(const T& x) {
    return std::log10(x);
}
template <std::floating_point T>
T utils::math<T>::hypot(const T& a, const T& b) {
    return std::hypot(a, b);
}
template <std::floating_point T>
T utils::math<T>::erf(const T& x) {
    return std::erf(x);
}
template <std::floating_point T>
T utils::math<T>::zeta(const T& x) {
    return std::riemann_zeta(x);
}
#ifdef __GSL_SF_EXPINT_H__
#include <gsl/gsl_errno.h>
#include <stdexcept>
#include <string>

template <std::floating_point T>
T utils::math<T>::ci_x(const T& x) {
    gsl_sf_result res;
    int status = gsl_sf_Ci_e(static_cast<double>(x), &res);
    if (status != GSL_SUCCESS) {
        throw std::runtime_error("GSL Error (Ci): " + std::string(gsl_strerror(status)));
    }
    return static_cast<T>(res.val);
}
template <std::floating_point T>
T utils::math<T>::si_x(const T& x) {
    gsl_sf_result res;
    int status = gsl_sf_Si_e(static_cast<double>(x), &res);
    if (status != GSL_SUCCESS) {
        throw std::runtime_error("GSL Error (Si): " + std::string(gsl_strerror(status)));
    }
    return static_cast<T>(res.val);
}
template <std::floating_point T>
T utils::math<T>::inc_gamma(const T& x, const T& alpha) {
    gsl_sf_result res;
    int status = gsl_sf_gamma_inc_e(static_cast<double>(alpha), static_cast<double>(x), &res);
    if (status != GSL_SUCCESS) {
        throw std::runtime_error("GSL Error (gamma_inc): " + std::string(gsl_strerror(status)));
    }
    return std::tgamma(alpha) - static_cast<T>(res.val);
}
template <std::floating_point T>
T utils::math<T>::lambertW0(const T& x) {
    gsl_sf_result res;
    int status = gsl_sf_lambert_W0_e(static_cast<double>(x), &res);
    if (status != GSL_SUCCESS) {
        throw std::runtime_error("GSL Error (lambert_W0): " + std::string(gsl_strerror(status)));
    }
    return static_cast<T>(res.val);
}
#endif

template <std::floating_point T>
T utils::math<T>::e_x(const T& x) {
    return std::comp_ellint_2(x);
}
template <std::floating_point T>
T utils::math<T>::k_x(const T& x) {
    return std::comp_ellint_1(x);
}

template <std::floating_point T>
T utils::math<T>::sin(const T& x) {
    return std::sin(x);
}
template <std::floating_point T>
T utils::math<T>::asin(const T& x) {
    return std::asin(x);
}
template <std::floating_point T>
T utils::math<T>::cos(const T& x) {
    return std::cos(x);
}
template <std::floating_point T>
T utils::math<T>::acos(const T& x) {
    return std::acos(x);
}
template <std::floating_point T>
T utils::math<T>::tan(const T& x) {
    return std::tan(x);
}
template <std::floating_point T>
T utils::math<T>::atan(const T& x) {
    return std::atan(x);
}
template <std::floating_point T>
T utils::math<T>::sinh(const T& x) {
    return std::sinh(x);
}
template <std::floating_point T>
T utils::math<T>::asinh(const T& x) {
    return std::asinh(x);
}
template <std::floating_point T>
T utils::math<T>::cosh(const T& x) {
    return std::cosh(x);
}
template <std::floating_point T>
T utils::math<T>::acosh(const T& x) {
    return std::acosh(x);
}
template <std::floating_point T>
T utils::math<T>::tanh(const T& x) {
    return std::tanh(x);
}
template <std::floating_point T>
T utils::math<T>::atanh(const T& x) {
    return std::atanh(x);
}
template <std::floating_point T>
T utils::math<T>::abs(const T& x) {
    return std::abs(x);
}

#endif