
/**
* @brief Maclaurin series of x / (1 - x)^2
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class x_1mx_squared_series final : public series_base<T, K>
{
public:
    x_1mx_squared_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit x_1mx_squared_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of x / (1 - x)^2
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
x_1mx_squared_series<T, K>::x_1mx_squared_series(T x) : series_base<T, K>(x, x / std::fma(x, x - 1, 1 - x))
{
    if (std::abs(this->x) > 1 || this->x == 1)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T x_1mx_squared_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(std::pow(this->x, n) * n);
}
