
/**
* @brief Trigonometric series of 1/12 * (3x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class one_twelfth_3x2_pi2_series final : public series_base<T, K>
{
public:
    one_twelfth_3x2_pi2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit one_twelfth_3x2_pi2_series(T x);

    /**
    * @brief Computes the nth term of the Trigonometric series of 1/12 * (3x^2 - pi^2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_twelfth_3x2_pi2_series<T, K>::one_twelfth_3x2_pi2_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(0.25 * x, x, -std::numbers::pi * std::numbers::pi / 12)))
{
    if (std::abs(this->x) > std::numbers::pi)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T one_twelfth_3x2_pi2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? series_base<T, K>::minus_one_raised_to_power_n(n) * std::cos(n * this->x) / (n * n) : 0;
}
