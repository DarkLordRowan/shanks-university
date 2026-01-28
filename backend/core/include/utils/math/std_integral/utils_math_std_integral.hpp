#ifndef UTILS_MATH_STD_INTEGRAL_HPP
#define UTILS_MATH_STD_INTEGRAL_HPP
#pragma once

template <std::integral T>
struct utils::math<T> {
    static T phi(T n);
    static T fact(const T n);
    static T double_fact(const T n);
    static T binomial_coefficient(const T n, const T k);
    static T minus_one_raised_to_power_n(const T j);
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
    static T ci_x(const T& x);
    static T si_x(const T& x);
    static T e_x(const T& x);
    static T k_x(const T& x);
    static T inc_gamma(const T& x, const T& alpha);
    static T lambertW0(const T& x);
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
}

template <std::integral T>
T utils::math<T>::phi(T n) {
    static_assert(std::false_type{}, "utils::math::phi not implemented for type");
}
template <std::integral T>
T utils::math<T>::fact(const T n) {
    static_assert(std::false_type{}, "utils::math::fact not implemented for type");
}
template <std::integral T>
T utils::math<T>::double_fact(const T n) {
    static_assert(std::false_type{}, "utils::math::double_fact not implemented for type");
}
template <std::integral T>
T utils::math<T>::binomial_coefficient(const T n, const T k) {
    static_assert(std::false_type{}, "utils::math::binomial_coefficient not implemented for type");
}
template <std::integral T>
T utils::math<T>::minus_one_raised_to_power_n(const T j) {
    if constexpr (std::is_signed<T>::value)
        return static_cast<T>(std::abs(j) & 1 ? -1 : 1);
    else
        static_assert(std::false_type{},
                      "utils::math::minus_one_raised_to_power_n can't be implemented for usigned integral type");
}
template <std::integral T>
T utils::math<T>::pow(const T& x, const T& y) {
    return std::pow(x, y);
}
template <std::integral T>
T utils::math<T>::atan2(const T& x, const T& y) {
    static_assert(std::false_type{}, "utils::math::atan2 not implemented for type");
}
template <std::integral T>
T utils::math<T>::fma(const T& a, const T& b, const T& c) {
    static_assert(std::false_type{}, "utils::math::fma not implemented for type");
}
template <std::integral T>
T utils::math<T>::sqrt(const T& x) {
    static_assert(std::false_type{}, "utils::math::sqrt not implemented for type");
}
template <std::integral T>
T utils::math<T>::exp(const T& x) {
    static_assert(std::false_type{}, "utils::math::exp not implemented for type");
}
template <std::integral T>
T utils::math<T>::log(const T& x) {
    static_assert(std::false_type{}, "utils::math::log not implemented for type");
}
template <std::integral T>
T utils::math<T>::log10(const T& x) {
    static_assert(std::false_type{}, "utils::math::log10 not implemented for type");
}
template <std::integral T>
T utils::math<T>::hypot(const T& a, const T& b) {
    static_assert(std::false_type{}, "utils::math::hypot not implemented for type");
}
template <std::integral T>
T utils::math<T>::erf(const T& x) {
    static_assert(std::false_type{}, "utils::math::erf not implemented for type");
}
template <std::integral T>
T utils::math<T>::zeta(const T& x) {
    static_assert(std::false_type{}, "utils::math::zeta not implemented for type");
}
template <std::integral T>
T utils::math<T>::ci_x(const T& x) {
    static_assert(std::false_type{}, "utils::math::ci_x not implemented for type");
}
template <std::integral T>
T utils::math<T>::si_x(const T& x) {
    static_assert(std::false_type{}, "utils::math::si_x not implemented for type");
}
template <std::integral T>
T utils::math<T>::e_x(const T& x) {
    static_assert(std::false_type{}, "utils::math::e_x not implemented for type");
}
template <std::integral T>
T utils::math<T>::k_x(const T& x) {
    static_assert(std::false_type{}, "utils::math::k_x not implemented for type");
}
template <std::integral T>
T utils::math<T>::inc_gamma(const T& x, const T& alpha) {
    static_assert(std::false_type{}, "utils::math::inc_gamma not implemented for type");
}
template <std::integral T>
T utils::math<T>::lambertW0(const T& x) {
    static_assert(std::false_type{}, "utils::math::lambertW0 not implemented for type");
}
template <std::integral T>
T utils::math<T>::sin(const T& x) {
    static_assert(std::false_type{}, "utils::math::sin not implemented for type");
}
template <std::integral T>
T utils::math<T>::asin(const T& x) {
    static_assert(std::false_type{}, "utils::math::asin not implemented for type");
}
template <std::integral T>
T utils::math<T>::cos(const T& x) {
    static_assert(std::false_type{}, "utils::math::cos not implemented for type");
}
template <std::integral T>
T utils::math<T>::acos(const T& x) {
    static_assert(std::false_type{}, "utils::math::acos not implemented for type");
}
template <std::integral T>
T utils::math<T>::tan(const T& x) {
    static_assert(std::false_type{}, "utils::math::tan not implemented for type");
}
template <std::integral T>
T utils::math<T>::atan(const T& x) {
    static_assert(std::false_type{}, "utils::math::atan not implemented for type");
}
template <std::integral T>
T utils::math<T>::sinh(const T& x) {
    static_assert(std::false_type{}, "utils::math::sinh not implemented for type");
}
template <std::integral T>
T utils::math<T>::asinh(const T& x) {
    static_assert(std::false_type{}, "utils::math::asinh not implemented for type");
}
template <std::integral T>
T utils::math<T>::cosh(const T& x) {
    static_assert(std::false_type{}, "utils::math::cosh not implemented for type");
}
template <std::integral T>
T utils::math<T>::acosh(const T& x) {
    static_assert(std::false_type{}, "utils::math::acosh not implemented for type");
}
template <std::integral T>
T utils::math<T>::tanh(const T& x) {
    static_assert(std::false_type{}, "utils::math::tanh not implemented for type");
}
template <std::integral T>
T utils::math<T>::atanh(const T& x) {
    static_assert(std::false_type{}, "utils::math::atanh not implemented for type");
}
template <std::integral T>
T utils::math<T>::abs(const T& x) {
    if constexpr (std::is_signed<T>::value) return std::abs(x) else return x;
}

#endif