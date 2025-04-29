
/**
* @brief Maclaurin series of function (pi - x) / 2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class pi_minus_x_2 final : public series_base<T, K>
{
public:
    pi_minus_x_2() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit pi_minus_x_2(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_minus_x_2<T, K>::pi_minus_x_2(T x) : series_base<T, K>(x, static_cast<T>((std::numbers::pi - x) / 2)) {}

template <typename T, typename K>
T pi_minus_x_2<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
        throw std::domain_error("The x value must be between 0 and 2*pi");
    return std::sin((n + 1) * (this->x)) / (n + 1);
}
