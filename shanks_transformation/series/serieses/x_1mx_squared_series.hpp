#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of function x / (1 - x)^2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_1mx_squared_series final : public series_base<T, K>
{
public:
    x_1mx_squared_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_1mx_squared_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of x / (1 - x)^2
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_1mx_squared_series<T, K>::x_1mx_squared_series(T x) : series_base<T, K>(x, x / static_cast<T>(std::fma(x, x - 1, 1 - x)))
{
    this->series_name = "x/(1-x)^2";
    // Сходится при |x| < 1 (производная геометрического ряда)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_1mx_squared_series<T, K>::operator()(K n) const
{
    return static_cast<T>(std::pow(this->x, n)) * static_cast<T>(n); // (14.4) [Rows.pdf]
}
