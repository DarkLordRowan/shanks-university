
/**
* @brief Fourier series of system functions f(x) = -3/4, -3 < x <= 0
*											f(x) = x - 3/4, 0 < x < 3
*											f(x + 6) = f(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class minus_3_div_4_or_x_minus_3_div_4_series final : public series_base<T, K>
{
public:
    minus_3_div_4_or_x_minus_3_div_4_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit minus_3_div_4_or_x_minus_3_div_4_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= -3 or this->x >= 3)
        throw std::domain_error("The value x must be between -3 and 3 not inclusive");
    return static_cast<T>(-6 / (std::numbers::pi * std::numbers::pi) * std::cos((2 * n + 1) * std::numbers::pi * this->x / 3) / ((2 * n + 1) * (2 * n + 1))
        - 3 / std::numbers::pi * ((1 - 2 * ((n + 1) % 2)) * std::sin(this->x * (n + 1) * std::numbers::pi / 3)) / (n + 1));  // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
}
