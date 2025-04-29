
/**
* @brief Series of function sqrt((1 - sqrt(1 - x)) / x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class sqrt_oneminussqrtoneminusx_div_x final : public series_base<T, K>
{
public:
    sqrt_oneminussqrtoneminusx_div_x() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit sqrt_oneminussqrtoneminusx_div_x(T x);

    /**
    * @brief Computes the nth term of the series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
sqrt_oneminussqrtoneminusx_div_x<T, K>::sqrt_oneminussqrtoneminusx_div_x(T x) : series_base<T, K>(x, std::sqrt((1 - sqrt(1 - x)) / x)) {}

template <typename T, typename K>
T sqrt_oneminussqrtoneminusx_div_x<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (std::abs(this->x) >= 1 or this->x == 0)
        throw std::domain_error("Modulus of the value x must be less 1 and cannot be equal to 0");
    return static_cast<T>(this->fact(4 * n) * pow(this->x, n) / (pow(2, 4 * n) * sqrt(2) * this->fact(2 * n) * this->fact(2 * n + 1)));
}

