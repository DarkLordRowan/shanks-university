
/**
* @brief Maclaurin series of (1 - 4x) ^ (-1/2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class inverse_sqrt_1m4x_series final : public series_base<T, K>
{
public:
    inverse_sqrt_1m4x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit inverse_sqrt_1m4x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of (1 - 4x) ^ (-1/2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
inverse_sqrt_1m4x_series<T, K>::inverse_sqrt_1m4x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(std::fma(-4, x, 1), -0.5)))
{
    if (std::abs(this->x) > 0.25 || this->x == 0.25)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T inverse_sqrt_1m4x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const auto _fact_n = this->fact(n);
    return this->fact(2 * n) * static_cast<T>(pow(this->x, n) / (_fact_n * _fact_n));
}
