#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function arcsin(x) - x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsin_x_minus_x_series final : public series_base<T, K>
{
public:
    arcsin_x_minus_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arcsin_x_minus_x_series(T x);

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
arcsin_x_minus_x_series<T, K>::arcsin_x_minus_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::asin(x)) - x)
{
    this->series_name = "arcsin(x)-x";
    // Сходится при |x| ≤ 1 (ряд для arcsin(x) сходится при |x| ≤ 1)
    // Расходится при |x| > 1

    if (std::abs(x) > 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsin_x_minus_x_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 1));
    return (static_cast<T>(this->double_fact(a)) * static_cast<T>(std::pow(this->x, a+2))) / (static_cast<T>(this->double_fact(a+1)) * static_cast<T>(a+2)); // (55.3) [Rows.pdf]
}
