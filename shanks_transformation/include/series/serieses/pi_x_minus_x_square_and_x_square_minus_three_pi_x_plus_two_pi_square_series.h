#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

/**
* @brief Fourier series of system functions pi*x - x^2, 0 < x < pi
*											x^2 - 3*pi*x + 2*pi^2, pi < x < 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series final : public series_base<T, K>
{
public:
    pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
        throw std::domain_error("Modulus of the value x must be between 0 and 2*pi not inclusive");
    return static_cast<T>((8 / std::numbers::pi) * (std::sin((2 * n + 1) * (this->x)) / ((2 * n + 1) * (2 * n + 1) * (2 * n + 1))));
}

