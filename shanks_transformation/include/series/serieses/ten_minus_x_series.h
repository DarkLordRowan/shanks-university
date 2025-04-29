
/**
* @brief Fourier series of function f(x) = 10 - x, 5 < x < 15
*									f(x + 10) = f(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class ten_minus_x_series final : public series_base<T, K>
{
public:
    ten_minus_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit ten_minus_x_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ten_minus_x_series<T, K>::ten_minus_x_series(T x) : series_base<T, K>(x, 10 - x) {}

template <typename T, typename K>
T ten_minus_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= 5 or this->x >= 15)
        throw std::domain_error("The value x must be between 5 and 15 not inclusive");
    return static_cast<T>(10 * (1 - 2 * ((n + 1) % 2)) * std::sin((n + 1) * this->x * std::numbers::pi / 5) / ((n + 1) * std::numbers::pi)); // (1 - 2 * ((n + 1) % 2)) = (-1)^{n+1}
}
