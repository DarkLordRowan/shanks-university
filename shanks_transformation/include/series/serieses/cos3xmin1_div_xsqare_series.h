

/**
* @brief Taylor series of function cos(3x - 1) / x^2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class cos3xmin1_div_xsqare_series final : public series_base<T, K>
{
public:
    cos3xmin1_div_xsqare_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit cos3xmin1_div_xsqare_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (std::cos(3 * x) - 1) / (x * x)) {}

template <typename T, typename K>
T cos3xmin1_div_xsqare_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(this->minus_one_raised_to_power_n(n + 1) * std::pow(3, 2 * n + 2) * std::pow(this->x, 2 * n) / this->fact(2 * n + 2));
}

