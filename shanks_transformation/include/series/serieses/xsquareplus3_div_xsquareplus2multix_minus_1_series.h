
/**
* @brief Taylor series of function ((x^2 + 3) / (x^2 + 2*x)) - 1
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series final : public series_base<T, K>
{
public:
    xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit xsquareplus3_div_xsquareplus2multix_minus_1_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x) : series_base<T, K>(x, (x* x + 3) / (x * (x + 2)) - 1) {}

template <typename T, typename K>
T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (std::abs(this->x - 1) <= 3)
        throw std::domain_error("The absolute value of x - 1 must be greater than 3");
    return static_cast<T>(std::pow(-1, n) * (1.5 - 3.5 * std::pow(3, n)) / std::pow(this->x - 1, n + 1));
}
