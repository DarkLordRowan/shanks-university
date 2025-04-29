
/**
* @brief Maclaurin series of function ln(sin(x)) - ln(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class ln_sinx_minus_ln_x final : public series_base<T, K>
{
public:
    ln_sinx_minus_ln_x() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit ln_sinx_minus_ln_x(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln_sinx_minus_ln_x<T, K>::ln_sinx_minus_ln_x(T x) : series_base<T, K>(x, std::log(std::sin(x) / x)) {}

template <typename T, typename K>
T ln_sinx_minus_ln_x<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    return static_cast<T>(std::log(1 - (this->x) * (this->x) / ((n + 1) * (n + 1) * std::numbers::pi * std::numbers::pi)));
}
