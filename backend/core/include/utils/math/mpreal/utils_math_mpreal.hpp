#ifndef UTILS_MATH_MPREAL_HPP
#define UTILS_MATH_MPREAL_HPP
#pragma once

#ifdef __MPREAL_H__

template <>
struct utils::math<mpfr::mpreal> {
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
#ifdef __GSL_SF_EXPINT_H__
    static mpfr::mpreal ci_x(const mpfr::mpreal& x);
    static mpfr::mpreal si_x(const mpfr::mpreal& x);
    static mpfr::mpreal lambertW0(const mpfr::mpreal& x);
#endif
    static mpfr::mpreal e_x(const mpfr::mpreal& x);
    static mpfr::mpreal k_x(const mpfr::mpreal& x);
    static mpfr::mpreal inc_gamma(const mpfr::mpreal& x, const mpfr::mpreal& alpha);
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

    using has_erf = std::true_type;
    using has_zeta = std::true_type;
    using has_inc_gamma = std::true_type;
#ifdef __GSL_SF_EXPINT_H__
    using has_ci_x = std::true_type;
    using has_si_x = std::true_type;
    using has_lambertW0 = std::true_type;
#else
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
    using has_lambertW0 = std::false_type;
#endif
    using has_e_x = std::true_type;
    using has_k_x = std::true_type;
};

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
#ifdef __GSL_SF_EXPINT_H__
mpfr::mpreal utils::math<mpfr::mpreal>::ci_x(const mpfr::mpreal& x) {
    return mpfr::mpreal(gsl_sf_Ci(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::si_x(const mpfr::mpreal& x) {
    return mpfr::mpreal(gsl_sf_Si(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::lambertW0(const mpfr::mpreal& x) {
    return mpfr::mpreal(gsl_sf_lambert_W0(static_cast<double>(x)), x.get_prec());
}
#endif

mpfr::mpreal utils::math<mpfr::mpreal>::e_x(const mpfr::mpreal& x) {
    return mpfr::mpreal(std::comp_ellint_2(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::k_x(const mpfr::mpreal& x) {
    return mpfr::mpreal(std::comp_ellint_1(static_cast<double>(x)), x.get_prec());
}

mpfr::mpreal utils::math<mpfr::mpreal>::inc_gamma(const mpfr::mpreal& x, const mpfr::mpreal& alpha) {
    return mpfr::tgamma(alpha) - mpfr::gammainc(alpha, x);
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

mpfr::mpreal utils::math<mpfr::mpreal>::abs(const mpfr::mpreal& x) { return mpfr::abs(x); }

#endif

#endif