
/**
* @brief Maclaurin series of function ln(1 + sqrt(1 + x^2)) - ln(2)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class ln_1plussqrt1plusxsquare_minus_ln_2 final : public series_base<T, K>
{
public:
    ln_1plussqrt1plusxsquare_minus_ln_2() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit ln_1plussqrt1plusxsquare_minus_ln_2(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2(T x) : series_base<T, K>(x, static_cast<T>(std::log((1 + std::hypot(1, x)) / 2))) {}

template <typename T, typename K>
T ln_1plussqrt1plusxsquare_minus_ln_2<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if ((this->x) * (this->x) > 1)
        throw std::domain_error("x^2 cannot be more than 1");
    return static_cast<T>((this->minus_one_raised_to_power_n(n) * this->fact(2 * n + 1) * std::pow(this->x, 2 * n + 2)) / (pow(2, 2 * n + 2) * this->fact(n + 1) * this->fact(n + 1)));
}
