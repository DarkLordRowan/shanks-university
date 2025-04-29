
/**
* @brief Fourier series of system functions pi - 3*pi/4, -pi < x < 0
*											pi - x - 3*pi/4, 0 <= x <= pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"


template <typename T, typename K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series final : public series_base<T, K>
{
public:
    pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= -std::numbers::pi or this->x > std::numbers::pi)
        throw std::domain_error("The value x must be between -pi not inclusive and pi inclusive");
    return static_cast<T>(std::cos((n + 1) * this->x) * (1 - (1 - 2 * ((n + 1) % 2))) / ((n + 1) * (n + 1) * std::numbers::pi) + (1 - 2 * ((n + 1) % 2)) * std::sin((n + 1) * this->x) / (n + 1)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
}

