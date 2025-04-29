#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

/**
* @brief Fourier series of function x, -pi <= x <= pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class x_series final : public series_base<T, K>
{
public:
    x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    x_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
x_series<T, K>::x_series(T x) : series_base<T, K>(x, this->x) {}

template <typename T, typename K>
constexpr T x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x < -std::numbers::pi or this->x > std::numbers::pi)
        throw std::domain_error("The value x must be between -pi and pi inclusive");
    return static_cast<T>(2 * std::pow(-1, n) * std::sin(this->x * (n + 1)) / (n + 1));
}

