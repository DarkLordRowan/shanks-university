

/**
* @brief Series of function (1 - sqrt(1 - 4x)) / 2x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class one_minus_sqrt_1minus4x_div_2x final : public series_base<T, K>
{
public:
    one_minus_sqrt_1minus4x_div_2x() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit one_minus_sqrt_1minus4x_div_2x(T x);

    /**
    * @brief Computes the nth term of the series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_minus_sqrt_1minus4x_div_2x<T, K>::one_minus_sqrt_1minus4x_div_2x(T x) : series_base<T, K>(x, (1 - static_cast<T>(sqrt(std::fma(-4, x, 1)))) / (2 * x)) {}

template <typename T, typename K>
T one_minus_sqrt_1minus4x_div_2x<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (std::abs(this->x) > 0.25 or this->x == 0)
        throw std::domain_error("Modulus of the value x must be less or equal 1/4 and cannot be equal to 0");

    T xn = static_cast<T>(std::pow(this->x, n));
    T binom_coef = static_cast<T>(this->binomial_coefficient(static_cast<T>(2 * n), n));
    T n_plus_1 = static_cast<T>(n + 1);

    return (xn * binom_coef) / n_plus_1;
}
