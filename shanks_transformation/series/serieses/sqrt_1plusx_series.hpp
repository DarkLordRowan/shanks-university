#pragma once
#include "series/series_base.hpp"

/**
* @brief Taylor series of function sqrt(1 + x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sqrt_1plusx_series final : public series_base<T, K>
{
public:
    sqrt_1plusx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @param x The argument for function series
    */
    sqrt_1plusx_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sqrt_1plusx_series<T, K>::sqrt_1plusx_series(T x) : series_base<T, K>(x, std::sqrt(1 + x))
{
    this->series_name = "sqrt(1+x)";
    // Сходится при |x| ≤ 1 (биномиальный ряд, абсолютно при |x| < 1, условно при x = -1)
    // Расходится при |x| > 1

    if (std::abs(x) > 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sqrt_1plusx_series<T, K>::operator()(K n) const
{
    if(n == 0)
        return static_cast<T>(1);

    const T coeff = static_cast<T>(this->double_fact(fma(2,n,1))) / static_cast<T>(this->double_fact(2*n));
    return this->minus_one_raised_to_power_n(n+1) * coeff * std::pow(this->x, static_cast<T>(n)) / static_cast<T>(fma(2,n,1)) / static_cast<T>(2*n-1); // (73.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}
