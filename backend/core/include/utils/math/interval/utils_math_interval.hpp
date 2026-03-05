#ifndef UTILS_MATH_INTERVAL_HPP
#define UTILS_MATH_INTERVAL_HPP
#pragma once

#ifdef INTERVALPRECISION_FWD_HPP

template <typename T>
struct utils::math<intprec::interval<T>> {
    template <std::integral K>
    static intprec::interval<T> minus_one_raised_to_power_n(const K j);
    static intprec::interval<T> pow(const intprec::interval<T>& x, const intprec::interval<T>& y);
    static intprec::interval<T> fma(const intprec::interval<T>& a, const intprec::interval<T>& b,
                                    const intprec::interval<T>& c);
    static intprec::interval<T> sqrt(const intprec::interval<T>& x);
    static intprec::interval<T> exp(const intprec::interval<T>& x);
    static intprec::interval<T> log(const intprec::interval<T>& x);
    static intprec::interval<T> log10(const intprec::interval<T>& x);
    static intprec::interval<T> hypot(const intprec::interval<T>& a, const intprec::interval<T>& b);
    static intprec::interval<T> sin(const intprec::interval<T>& x);
    static intprec::interval<T> asin(const intprec::interval<T>& x);
    static intprec::interval<T> cos(const intprec::interval<T>& x);
    static intprec::interval<T> acos(const intprec::interval<T>& x);
    static intprec::interval<T> tan(const intprec::interval<T>& x);
    static intprec::interval<T> atan(const intprec::interval<T>& x);
    static intprec::interval<T> sinh(const intprec::interval<T>& x);
    static intprec::interval<T> asinh(const intprec::interval<T>& x);
    static intprec::interval<T> cosh(const intprec::interval<T>& x);
    static intprec::interval<T> acosh(const intprec::interval<T>& x);
    static intprec::interval<T> tanh(const intprec::interval<T>& x);
    static intprec::interval<T> atanh(const intprec::interval<T>& x);
    static intprec::interval<T> abs(const intprec::interval<T>& x);

    using has_erf = std::false_type;
    using has_zeta = std::false_type;
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
    using has_e_x = std::false_type;
    using has_k_x = std::false_type;
    using has_inc_gamma = std::false_type;
    using has_lambertW0 = std::false_type;
};

template <typename T>
template <std::integral K>
intprec::interval<T> utils::math<intprec::interval<T>>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return intprec::interval<T>(std::abs(j) & 1 ? -1 : 1);
    else
        return intprec::interval<T>(j & 1 ? -1 : 1);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::pow(const intprec::interval<T>& x,
                                                            const intprec::interval<T>& y) {
    return intprec::pow(x, y);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::fma(const intprec::interval<T>& a,
                                                            const intprec::interval<T>& b,
                                                            const intprec::interval<T>& c) {
    return a * b + c;
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::sqrt(const intprec::interval<T>& x) {
    return intprec::sqrt(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::exp(const intprec::interval<T>& x) {
    return intprec::exp(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::log(const intprec::interval<T>& x) {
    return intprec::log(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::log10(const intprec::interval<T>& x) {
    return intprec::log10(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::hypot(const intprec::interval<T>& a,
                                                              const intprec::interval<T>& b) {
    return intprec::sqrt(a.sqr() + b.sqr);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::sin(const intprec::interval<T>& x) {
    return intprec::sin(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::asin(const intprec::interval<T>& x) {
    return intprec::asin(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::cos(const intprec::interval<T>& x) {
    return intprec::cos(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::acos(const intprec::interval<T>& x) {
    return intprec::acos(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::tan(const intprec::interval<T>& x) {
    return intprec::tan(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::atan(const intprec::interval<T>& x) {
    return intprec::atan(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::sinh(const intprec::interval<T>& x) {
    return intprec::sinh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::asinh(const intprec::interval<T>& x) {
    return intprec::asinh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::cosh(const intprec::interval<T>& x) {
    return intprec::cosh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::acosh(const intprec::interval<T>& x) {
    return intprec::acosh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::tanh(const intprec::interval<T>& x) {
    return intprec::tanh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::atanh(const intprec::interval<T>& x) {
    return intprec::atanh(x);
}
template <typename T>
intprec::interval<T> utils::math<intprec::interval<T>>::abs(const intprec::interval<T>& x) {
    return intprec::abs(x);
}

#endif

#endif