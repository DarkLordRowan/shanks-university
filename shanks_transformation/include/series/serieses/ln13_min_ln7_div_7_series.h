
/**
* @brief Fourier series of function (ln13 - ln7) / 7
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class ln13_min_ln7_div_7_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    */
    ln13_min_ln7_div_7_series();

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series() : series_base<T, K>(0, (static_cast<T>(std::log(13 / 7) / 7))) {}

template <typename T, typename K>
T ln13_min_ln7_div_7_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(6, n + 1) / ((n + 1) * std::pow(7, n + 2)));
}
