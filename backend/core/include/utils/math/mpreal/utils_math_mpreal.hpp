#ifndef UTILS_MATH_MPREAL_HPP
#define UTILS_MATH_MPREAL_HPP
#pragma once

template <>
struct utils::math<mpfr::mpreal> {
    static mpfr::mpreal phi(mpfr::mpreal n);
    static mpfr::mpreal fact(const mpfr::mpreal n);
    static mpfr::mpreal double_fact(const mpfr::mpreal n);
    static mpfr::mpreal binomial_coefficient(const mpfr::mpreal n, const mpfr::mpreal k);
    template <std::integral K>
    static mpfr::mpreal minus_one_raised_to_power_n(const K j);
    static mpfr::mpreal pow(const mpfr::mpreal& x, const mpfr::mpreal& y);
    static mpfr::mpreal atan2(const mpfr::mpreal& x, const mpfr::mpreal& y);
    static mpfr::mpreal fma(const mpfr::mpreal& a, const mpfr::mpreal& b, const mpfr::mpreal& c);
    static mpfr::mpreal sqrt(const mpfr::mpreal& x);
    static mpfr::mpreal exp(const mpfr::mpreal& x);
    static mpfr::mpreal log(const mpfr::mpreal& x);
    static mpfr::mpreal log10(const mpfr::mpreal& x);
    static mpfr::mpreal hypot(const mpfr::mpreal& a, const mpfr::mpreal& b);
    static mpfr::mpreal erf(const mpfr::mpreal& x);
    static mpfr::mpreal zeta(const mpfr::mpreal& x);
    static mpfr::mpreal ci_x(const mpfr::mpreal& x);
    static mpfr::mpreal si_x(const mpfr::mpreal& x);
    static mpfr::mpreal e_x(const mpfr::mpreal& x);
    static mpfr::mpreal k_x(const mpfr::mpreal& x);
    static mpfr::mpreal inc_gamma(const mpfr::mpreal& x, const mpfr::mpreal& alpha);
    static mpfr::mpreal lambertW0(const mpfr::mpreal& x);
    static mpfr::mpreal sin(const mpfr::mpreal& x);
    static mpfr::mpreal asin(const mpfr::mpreal& x);
    static mpfr::mpreal cos(const mpfr::mpreal& x);
    static mpfr::mpreal acos(const mpfr::mpreal& x);
    static mpfr::mpreal tan(const mpfr::mpreal& x);
    static mpfr::mpreal atan(const mpfr::mpreal& x);
    static mpfr::mpreal sinh(const mpfr::mpreal& x);
    static mpfr::mpreal asinh(const mpfr::mpreal& x);
    static mpfr::mpreal cosh(const mpfr::mpreal& x);
    static mpfr::mpreal acosh(const mpfr::mpreal& x);
    static mpfr::mpreal tanh(const mpfr::mpreal& x);
    static mpfr::mpreal atanh(const mpfr::mpreal& x);
    static mpfr::mpreal abs(const mpfr::mpreal& x);
}

mpfr::mpreal
utils::math<mpfr::mpreal>::phi(mpfr::mpreal n) {
    static_assert(std::false_type{}, "utils::math::phi not implemented for type");
}

mpfr::mpreal utils::math<mpfr::mpreal>::fact(const mpfr::mpreal n) {
    static_assert(std::false_type{}, "utils::math::fact not implemented for type");
}

mpfr::mpreal utils::math<mpfr::mpreal>::double_fact(const mpfr::mpreal n) {
    static_assert(std::false_type{}, "utils::math::double_fact not implemented for type");
}

mpfr::mpreal utils::math<mpfr::mpreal>::binomial_coefficient(const mpfr::mpreal n, const mpfr::mpreal k) {
    static_assert(std::false_type{}, "utils::math::binomial_coefficient not implemented for type");
}
template <std::integral K>
mpfr::mpreal utils::math<mpfr::mpreal>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return static_cast<mpfr::mpreal>(std::abs(j) & 1 ? -1 : 1);
    else
        return static_cast<mpfr::mpreal>(j & 1 ? -1 : 1);
}

mpfr::mpreal utils::math<mpfr::mpreal>::pow(const mpfr::mpreal& x, const mpfr::mpreal& y) { return mpfr::pow(x, y); }

mpfr::mpreal utils::math<mpfr::mpreal>::atan2(const mpfr::mpreal& x, const mpfr::mpreal& y) {
    return mpfr::atan2(x, y);
}

mpfr::mpreal utils::math<mpfr::mpreal>::fma(const mpfr::mpreal& a, const mpfr::mpreal& b, const mpfr::mpreal& c) {
    return mpfr::fma(a, b, c);
}

mpfr::mpreal utils::math<mpfr::mpreal>::sqrt(const mpfr::mpreal& x) { return mpfr::sqrt(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::exp(const mpfr::mpreal& x) { return mpfr::exp(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::log(const mpfr::mpreal& x) { return mpfr::log(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::log10(const mpfr::mpreal& x) { return mpfr::log10(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::hypot(const mpfr::mpreal& a, const mpfr::mpreal& b) {
    return mpfr::hypot(a, b);
}

mpfr::mpreal utils::math<mpfr::mpreal>::erf(const mpfr::mpreal& x) { return mpfr::erf(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::zeta(const mpfr::mpreal& x) { return mpfr::zeta(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::ci_x(const mpfr::mpreal& x) {
#ifdef __GSL_SF_EXPINT_H__
    return static_cast<mpfr::mpreal>(gsl_sf_Ci(static_cast<double>(x)), x.get_prec());
#else
    static_assert(std::false_type{}, "utils::math::ci_x not implemented for type");
#endif
}

mpfr::mpreal utils::math<mpfr::mpreal>::si_x(const mpfr::mpreal& x) {
#ifdef __GSL_SF_EXPINT_H__
    return static_cast<mpfr::mpreal>(gsl_sf_Si(static_cast<double>(x)), x.get_prec());
#else
    static_assert(std::false_type{}, "utils::math::si_x not implemented for type");
#endif
}

mpfr::mpreal utils::math<mpfr::mpreal>::e_x(const mpfr::mpreal& x) {
    return static_cast<mpfr::mpreal>(std::comp_ellint_2(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::k_x(const mpfr::mpreal& x) {
    return static_cast<mpfr::mpreal>(std::comp_ellint_1(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::inc_gamma(const mpfr::mpreal& x, const mpfr::mpreal& alpha) {
#ifdef __GSL_SF_EXPINT_H__
    return mpfr::tgamma(alpha) -
           static_cast<mpfr::mpreal>(gsl_sf_gamma_inc(static_cast<double>(alpha), static_cast<double>(x)),
                                     x.get_prec());
#else
    static_assert(std::false_type{}, "utils::math::inc_gamma not implemented for type");
#endif
}

mpfr::mpreal utils::math<mpfr::mpreal>::lambertW0(const mpfr::mpreal& x) {
#ifdef __GSL_SF_EXPINT_H__
    return static_cast<mpfr::mpreal>(gsl_sf_lambert_W0(static_cast<double>(x)), x.get_prec());
#else
    static_assert(std::false_type{}, "utils::math::lambertW0 not implemented for type");
#endif
}

mpfr::mpreal utils::math<mpfr::mpreal>::sin(const mpfr::mpreal& x) { return mpfr::sin(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::asin(const mpfr::mpreal& x) { return mpfr::asin(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::cos(const mpfr::mpreal& x) { return mpfr::cos(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::acos(const mpfr::mpreal& x) { return mpfr::acos(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::tan(const mpfr::mpreal& x) { return mpfr::tan(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::atan(const mpfr::mpreal& x) { return mpfr::atan(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::sinh(const mpfr::mpreal& x) { return mpfr::sinh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::asinh(const mpfr::mpreal& x) { return mpfr::asinh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::cosh(const mpfr::mpreal& x) { return mpfr::cosh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::acosh(const mpfr::mpreal& x) { return mpfr::acosh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::tanh(const mpfr::mpreal& x) { return mpfr::tanh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::atanh(const mpfr::mpreal& x) { return mpfr::atanh(x); }

mpfr::mpreal utils::math<mpfr::mpreal>::abs(const mpfr::mpreal& x) { return mpfr::abs(x) }

#endif