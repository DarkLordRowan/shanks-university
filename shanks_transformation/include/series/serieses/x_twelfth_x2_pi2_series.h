
/**
* @brief Trigonometric series of x/12 * (x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/


#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class x_twelfth_x2_pi2_series final : public series_base<T, K>
{
public:
    x_twelfth_x2_pi2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit x_twelfth_x2_pi2_series(T x);

    /**
    * @brief Computes the nth term of the Trigonometric series of x/12 * (x^2 - pi^2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(x / 12, (x + std::numbers::pi)* (x - std::numbers::pi), -std::fma(x + std::numbers::pi, x - std::numbers::pi, (x + std::numbers::pi) * (x - std::numbers::pi)))))
{
    if (std::abs(this->x) > std::numbers::pi)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::sin(n * this->x) / (std::pow(n, 3))) : 0;
}

