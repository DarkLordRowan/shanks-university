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
    static T fma(const T& a, const T& b, const T& c);
    static T abs(const T& x);

    // availability of special function
    using has_erf = std::false_type;
    using has_zeta = std::false_type;
    using has_ci_x = std::false_type;
    using has_si_x = std::false_type;
    using has_e_x = std::false_type;
    using has_k_x = std::false_type;
    using has_inc_gamma = std::false_type;
    using has_lambertW0 = std::false_type;
};

template <std::integral T>
T utils::math<T>::phi(const T n) {
    if (n < 0) throw std::invalid_argument("parameter n in phi cant be a negative number");
    T result = n;
    T temp_n = n;
    // Iterating to find prime factors and applying the formula
    for (T i = 2; i * i <= temp_n; ++i)
        if (temp_n % i == 0) {
            while (temp_n % i == 0) temp_n /= i;
            result -= result / i;
        }

    // Final step for the remaining prime factor
    result -= temp_n > 1 ? result / temp_n : 0;
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
                      "utils::math::minus_one_raised_to_power_n can't be implemented for unsigned integral type");
}
template <std::integral T>
T utils::math<T>::pow(const T& x, const T& y) {
    return std::pow(x, y);
}
template <std::integral T>
T utils::math<T>::fma(const T& a, const T& b, const T& c) {
    return std::fma(a, b, c);
}
template <std::integral T>
T utils::math<T>::abs(const T& x) {
    if constexpr (std::is_signed<T>::value)
        return std::abs(x);
    else
        return x;
}

#endif