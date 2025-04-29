
/**
* @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class pi_8_cosx_square_minus_1_div_3_cosx final : public series_base<T, K>
{
public:
    pi_8_cosx_square_minus_1_div_3_cosx() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit pi_8_cosx_square_minus_1_div_3_cosx(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_8_cosx_square_minus_1_div_3_cosx<T, K>::pi_8_cosx_square_minus_1_div_3_cosx(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi / 8)* std::cos(x)* (std::cos(x) - (1 / 3)))) {}

template <typename T, typename K>
T pi_8_cosx_square_minus_1_div_3_cosx<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (abs(this->x) > std::numbers::pi / 2)
        throw std::domain_error("The value x must be between -pi/2 and pi/2 inclusive");
    return static_cast<T>(this->minus_one_raised_to_power_n(n) * std::cos(2 * n + 3) * this->x / ((2 * n + 1) * (2 * n + 3) * (2 * n + 5)));
}