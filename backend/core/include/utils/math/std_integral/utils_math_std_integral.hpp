#ifndef UTILS_MATH_STD_INTEGRAL_HPP
#define UTILS_MATH_STD_INTEGRAL_HPP
#pragma once

template <std::integral T>
struct utils::math<T> {
    static T phi(const T n);
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
};

template <std::integral T>
T utils::math<T>::phi(const T n) {
    if (n < 0) throw std::invalid_argument("parameter n in phi cant be a negative number");
    T result = n;
    // Iterating to find prime factors and applying the formula
    for (T i = 2; i * i <= n; ++i)
        if (n % i == 0) {
            while (n % i == 0) n /= i;
            result -= result / i;
        }

    // Final step for the remaining prime factor
    result -= n > 1 ? result / n : 0;
    return result;
}
template <std::integral T>
T utils::math<T>::fact(const T n) {
    if (n < 0) throw std::invalid_argument("parameter n in fact cant be a negative number");
    T res = 1;
    for (T j = 2; j <= n; ++j) {
        res *= j;
    }
    return res;
}
template <std::integral T>
T utils::math<T>::double_fact(const T n) {
    if (n < 0) throw std::invalid_argument("parameter n in double_fact cant be a negative number");
    T res = 1;

    // Multiplies every second integer down to 1 or 2
    for (T j = n & 1 + 2; j <= n; j += 2) {
        res *= j;
    }

    return res;
}
template <std::integral T>
T utils::math<T>::binomial_coefficient(const T n, const T k) {
    if (n < 0) throw std::invalid_argument("parameter n in binomial_coefficient cant be a negative number");
    if (k < 0) throw std::invalid_argument("parameter k in binomial_coefficient cant be a negative number");

    if (n < k) throw std::invalid_argument("n cant be bigger than k");

    if (n == k || k == 0) return 1;

    // Using DP approach for stability and avoiding large intermediate values
    const T new_k = (k > (n + n % 2) / 2 ? n - k : k);
    std::vector<T> dp(new_k + 1);
    dp[0] = 1;
    for (T i = 1; i <= n; ++i)
        for (T j = (i > new_k ? new_k : i); j > 0; --j) dp[j] += dp[j - 1];

    return dp[new_k];
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
    return std::fma(a, b, c);
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
    if constexpr (std::is_signed<T>::value)
        return std::abs(x);
    else
        return x;
}

#endif