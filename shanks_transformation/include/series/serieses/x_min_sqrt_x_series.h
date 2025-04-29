

/**
* @brief Taylor series of function x - sqrt(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class x_min_sqrt_x_series final : public series_base<T, K>
{
public:
    x_min_sqrt_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    x_min_sqrt_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
x_min_sqrt_x_series<T, K>::x_min_sqrt_x_series(T x) : series_base<T, K>(x, x - std::sqrt(x)) {}

template <typename T, typename K>
T x_min_sqrt_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    T tempsum = 0;
    for (int m = 0; m < std::pow(2, n); m++)
        tempsum += static_cast<T>(static_cast<T>(std::pow(this->x, m)) * static_cast<T>(this->binomial_coefficient(static_cast<T>(std::pow(2, n + 1)), 2 * m + 1)));

    return static_cast<T>(std::pow(this->x - 1, std::pow(2, n)) / tempsum);
}

