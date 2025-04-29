
/**
* @brief Fourier series of system functions -x, -pi < x <= 0
*											 0,   0 < x < pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"


template <typename T, typename K>
class minus_x_minus_pi_4_or_minus_pi_4_series final : public series_base<T, K>
{
public:
    minus_x_minus_pi_4_or_minus_pi_4_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit minus_x_minus_pi_4_or_minus_pi_4_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= -std::numbers::pi or this->x >= std::numbers::pi)
        throw std::domain_error("The value x must be between -pi and pi not inclusive");
    return static_cast<T>(-2 * std::cos(2 * n + 1) * this->x / (std::numbers::pi * (2 * n + 1) * (2 * n + 1))
        + (1 - 2 * ((n + 1) % 2)) * std::sin(this->x * (n + 1)) / (n + 1)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
}
