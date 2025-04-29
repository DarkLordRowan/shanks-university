
/**
* @brief Fourier series of system functions sin(x) - 2/pi, 0 <= x <= pi
*											-sin(x) - 2/pi, pi <= x <= 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class abs_sin_x_minus_2_div_pi_series final : public series_base<T, K>
{
public:
    abs_sin_x_minus_2_div_pi_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit abs_sin_x_minus_2_div_pi_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(T x) : series_base<T, K>(x, static_cast<T>(std::abs(std::sin(x)) - (2 / std::numbers::pi))) {}

template <typename T, typename K>
T abs_sin_x_minus_2_div_pi_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(-4 * (std::cos(2 * (this->x) * (n + 1))) / ((2 * n + 1) * (2 * n + 3) * std::numbers::pi));
}

