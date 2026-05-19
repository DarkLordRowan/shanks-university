#ifndef UTILS_MATH_STD_COMPLEX_HPP
#define UTILS_MATH_STD_COMPLEX_HPP
#pragma once

template <typename T>
struct utils::math<std::complex<T>> {
    template <std::integral K>
    static std::complex<T> minus_one_raised_to_power_n(const K j);
    static std::complex<T> pow(const std::complex<T>& x, const std::complex<T>& y);
    static std::complex<T> atan2(const std::complex<T>& x, const std::complex<T>& y);
    static std::complex<T> fma(const std::complex<T>& a, const std::complex<T>& b, const std::complex<T>& c);
    static std::complex<T> sqrt(const std::complex<T>& x);
    static std::complex<T> exp(const std::complex<T>& x);
    static std::complex<T> log(const std::complex<T>& x);
    static std::complex<T> log10(const std::complex<T>& x);
    static std::complex<T> hypot(const std::complex<T>& a, const std::complex<T>& b);
    static std::complex<T> sin(const std::complex<T>& x);
    static std::complex<T> asin(const std::complex<T>& x);
    static std::complex<T> cos(const std::complex<T>& x);
    static std::complex<T> acos(const std::complex<T>& x);
    static std::complex<T> tan(const std::complex<T>& x);
    static std::complex<T> atan(const std::complex<T>& x);
    static std::complex<T> sinh(const std::complex<T>& x);
    static std::complex<T> asinh(const std::complex<T>& x);
    static std::complex<T> cosh(const std::complex<T>& x);
    static std::complex<T> acosh(const std::complex<T>& x);
    static std::complex<T> tanh(const std::complex<T>& x);
    static std::complex<T> atanh(const std::complex<T>& x);
    static T real(const std::complex<T>& x);
    static T imag(const std::complex<T>& x);
    static T abs(const std::complex<T>& x);

    // availability of special function
    using has_erf = std::false_type;
    using has_zeta = std::false_type;
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
    using has_e_x = std::false_type;
    using has_k_x = std::false_type;
    using has_inc_gamma = std::false_type;
    using has_lambertW0 = std::false_type;
    using has_airy_ai = std::false_type;
    using has_clausen = std::false_type;
    using has_bessel_first_kind = std::false_type;
};

template <typename T>
template <std::integral K>
std::complex<T> utils::math<std::complex<T>>::minus_one_raised_to_power_n(const K j) {
    if constexpr (std::is_signed<K>::value)
        return std::complex<T>(std::abs(j) & 1 ? -1 : 1);
    else
        return std::complex<T>(j & 1 ? -1 : 1);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::pow(const std::complex<T>& x, const std::complex<T>& y) {
    return std::pow(x, y);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::atan2(const std::complex<T>& x, const std::complex<T>& y) {
    return std::atan2(x, y);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::fma(const std::complex<T>& a, const std::complex<T>& b,
                                                  const std::complex<T>& c) {
    return a * b + c;
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::sqrt(const std::complex<T>& x) {
    return std::sqrt(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::exp(const std::complex<T>& x) {
    return std::exp(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::log(const std::complex<T>& x) {
    return std::log(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::log10(const std::complex<T>& x) {
    return std::log10(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::hypot(const std::complex<T>& a, const std::complex<T>& b) {
    return std::hypot(a, b);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::sin(const std::complex<T>& x) {
    return std::sin(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::asin(const std::complex<T>& x) {
    return std::asin(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::cos(const std::complex<T>& x) {
    return std::cos(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::acos(const std::complex<T>& x) {
    return std::acos(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::tan(const std::complex<T>& x) {
    return std::tan(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::atan(const std::complex<T>& x) {
    return std::atan(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::sinh(const std::complex<T>& x) {
    return std::sinh(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::asinh(const std::complex<T>& x) {
    return std::asinh(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::cosh(const std::complex<T>& x) {
    return std::cosh(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::acosh(const std::complex<T>& x) {
    return std::acosh(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::tanh(const std::complex<T>& x) {
    return std::tanh(x);
}
template <typename T>
std::complex<T> utils::math<std::complex<T>>::atanh(const std::complex<T>& x) {
    return std::atanh(x);
}
template <typename T>
T utils::math<std::complex<T>>::real(const std::complex<T>& x) {
    return x.real();
}
template <typename T>
T utils::math<std::complex<T>>::imag(const std::complex<T>& x) {
    return x.imag();
}

template <typename T>
T utils::math<std::complex<T>>::abs(const std::complex<T>& x) {
    if constexpr (is_profiling<T>::value)
        return (x * std::conj(x)).real();
    else
        return std::abs(x);
}

#endif