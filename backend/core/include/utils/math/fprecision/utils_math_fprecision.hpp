#ifndef UTILS_MATH_FPRECISION_HPP
#define UTILS_MATH_FPRECISION_HPP
#pragma once

#ifdef INC_FPRECISION

template <>
struct utils::math<arb::float_precision> {
    template <std::integral K>
    static arb::float_precision minus_one_raised_to_power_n(const K j);
    static arb::float_precision pow(const arb::float_precision& x, const arb::float_precision& y);
    static arb::float_precision atan2(const arb::float_precision& x, const arb::float_precision& y);
    static arb::float_precision fma(const arb::float_precision& a, const arb::float_precision& b,
                                    const arb::float_precision& c);
    static arb::float_precision sqrt(const arb::float_precision& x);
    static arb::float_precision exp(const arb::float_precision& x);
    static arb::float_precision log(const arb::float_precision& x);
    static arb::float_precision log10(const arb::float_precision& x);
    static arb::float_precision hypot(const arb::float_precision& a, const arb::float_precision& b);
    static arb::float_precision erf(const arb::float_precision& x);
    static arb::float_precision zeta(const arb::float_precision& x);
#ifdef __GSL_SF_EXPINT_H__
    static arb::float_precision ci_x(const arb::float_precision& x);
    static arb::float_precision si_x(const arb::float_precision& x);
    static arb::float_precision lambertW0(const arb::float_precision& x);
#endif
    static arb::float_precision e_x(const arb::float_precision& x);
    static arb::float_precision k_x(const arb::float_precision& x);
    static arb::float_precision sin(const arb::float_precision& x);
    static arb::float_precision asin(const arb::float_precision& x);
    static arb::float_precision cos(const arb::float_precision& x);
    static arb::float_precision acos(const arb::float_precision& x);
    static arb::float_precision tan(const arb::float_precision& x);
    static arb::float_precision atan(const arb::float_precision& x);
    static arb::float_precision sinh(const arb::float_precision& x);
    static arb::float_precision asinh(const arb::float_precision& x);
    static arb::float_precision cosh(const arb::float_precision& x);
    static arb::float_precision acosh(const arb::float_precision& x);
    static arb::float_precision tanh(const arb::float_precision& x);
    static arb::float_precision atanh(const arb::float_precision& x);
    static arb::float_precision abs(const arb::float_precision& x);

    using has_erf = std::true_type;
    using has_zeta = std::true_type;
    using has_inc_gamma = std::false_type;
    using has_lambertW0 = std::true_type;
#ifdef __GSL_SF_EXPINT_H__
    using has_ci_x = std::true_type;
    using has_si_x = std::true_type;
#else
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
#endif
    using has_e_x = std::true_type;
    using has_k_x = std::true_type;
};

template <std::integral K>
inline arb::float_precision utils::math<arb::float_precision>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return static_cast<arb::float_precision>(std::abs(j) & 1 ? -1 : 1);
    else
        return static_cast<arb::float_precision>(j & 1 ? -1 : 1);
}

inline arb::float_precision utils::math<arb::float_precision>::pow(const arb::float_precision& x,
                                                                   const arb::float_precision& y) {
    return arb::pow(x, y);
}

inline arb::float_precision utils::math<arb::float_precision>::atan2(const arb::float_precision& x,
                                                                     const arb::float_precision& y) {
    return arb::atan2(x, y);
}

inline arb::float_precision utils::math<arb::float_precision>::fma(const arb::float_precision& a,
                                                                   const arb::float_precision& b,
                                                                   const arb::float_precision& c) {
    return arb::fma(a, b, c);
}

inline arb::float_precision utils::math<arb::float_precision>::sqrt(const arb::float_precision& x) {
    return arb::sqrt(x);
}

inline arb::float_precision utils::math<arb::float_precision>::exp(const arb::float_precision& x) {
    return arb::exp(x);
}

inline arb::float_precision utils::math<arb::float_precision>::log(const arb::float_precision& x) {
    return arb::log(x);
}

inline arb::float_precision utils::math<arb::float_precision>::log10(const arb::float_precision& x) {
    return arb::log10(x);
}

inline arb::float_precision utils::math<arb::float_precision>::hypot(const arb::float_precision& a,
                                                                     const arb::float_precision& b) {
    return arb::sqrt(a * a + b * b);
}

inline arb::float_precision utils::math<arb::float_precision>::erf(const arb::float_precision& x) {
    return arb::erf(x);
}

inline arb::float_precision utils::math<arb::float_precision>::zeta(const arb::float_precision& x) {
    return arb::zeta(x);
}

#ifdef __GSL_SF_EXPINT_H__
inline arb::float_precision utils::math<arb::float_precision>::ci_x(const arb::float_precision& x) {
    return arb::float_precision(gsl_sf_Ci(static_cast<double>(x)), x.precision());
}

inline arb::float_precision utils::math<arb::float_precision>::si_x(const arb::float_precision& x) {
    return arb::float_precision(gsl_sf_Si(static_cast<double>(x)), x.precision());
}
#endif

inline arb::float_precision utils::math<arb::float_precision>::lambertW0(const arb::float_precision& x) {
    return arb::lambertW0(x);
}

inline arb::float_precision utils::math<arb::float_precision>::e_x(const arb::float_precision& x) {
    return arb::float_precision(std::comp_ellint_2(static_cast<double>(x)), x.precision());
}

inline arb::float_precision utils::math<arb::float_precision>::k_x(const arb::float_precision& x) {
    return arb::float_precision(std::comp_ellint_1(static_cast<double>(x)), x.precision());
}

inline arb::float_precision utils::math<arb::float_precision>::sin(const arb::float_precision& x) {
    return arb::sin(x);
}

inline arb::float_precision utils::math<arb::float_precision>::asin(const arb::float_precision& x) {
    return arb::asin(x);
}

inline arb::float_precision utils::math<arb::float_precision>::cos(const arb::float_precision& x) {
    return arb::cos(x);
}

inline arb::float_precision utils::math<arb::float_precision>::acos(const arb::float_precision& x) {
    return arb::acos(x);
}

inline arb::float_precision utils::math<arb::float_precision>::tan(const arb::float_precision& x) {
    return arb::tan(x);
}

inline arb::float_precision utils::math<arb::float_precision>::atan(const arb::float_precision& x) {
    return arb::atan(x);
}

inline arb::float_precision utils::math<arb::float_precision>::sinh(const arb::float_precision& x) {
    return arb::sinh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::asinh(const arb::float_precision& x) {
    return arb::asinh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::cosh(const arb::float_precision& x) {
    return arb::cosh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::acosh(const arb::float_precision& x) {
    return arb::acosh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::tanh(const arb::float_precision& x) {
    return arb::tanh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::atanh(const arb::float_precision& x) {
    return arb::atanh(x);
}

inline arb::float_precision utils::math<arb::float_precision>::abs(const arb::float_precision& x) {
    return arb::abs(x);
}

#endif

#endif