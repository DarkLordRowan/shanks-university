
/**
* @brief Maclaurin series of function 0.5 - (pi/4)*sin(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class half_minus_sinx_multi_pi_4 final : public series_base<T, K>
{
public:
    half_minus_sinx_multi_pi_4() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit half_minus_sinx_multi_pi_4(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
half_minus_sinx_multi_pi_4<T, K>::half_minus_sinx_multi_pi_4(T x) : series_base<T, K>(x, static_cast<T>(static_cast<T>(0.5) - std::numbers::pi * std::sin(x) * static_cast<T>(0.25))) {}

template <typename T, typename K>
T half_minus_sinx_multi_pi_4<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return std::cos(2 * this->x * (n + 1)) / ((2 * n + 1) * (2 * n + 3));
}
