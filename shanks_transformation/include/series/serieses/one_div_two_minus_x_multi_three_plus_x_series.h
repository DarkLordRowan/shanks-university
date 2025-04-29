
/**
* @brief Laurent series of function 1 / ((2 - x)*(3 + x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class one_div_two_minus_x_multi_three_plus_x_series final : public series_base<T, K>
{
public:
    one_div_two_minus_x_multi_three_plus_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit one_div_two_minus_x_multi_three_plus_x_series(T x);

    /**
    * @brief Computes the nth term of the Laurent series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Laurent series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_div_two_minus_x_multi_three_plus_x_series<T, K>::one_div_two_minus_x_multi_three_plus_x_series(T x) : series_base<T, K>(x, 1 / ((2 - x) * (3 + x))) {}

template <typename T, typename K>
T one_div_two_minus_x_multi_three_plus_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>((std::pow(-3, n) - std::pow(2, n)) / (5 * std::pow(this->x, n + 1)));
}
