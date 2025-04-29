
/**
* @brief Numerical series representation of pi^2 / 6 - 1
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class pi_squared_6_minus_one_series final : public series_base<T, K>
{
public:
    pi_squared_6_minus_one_series();

    /**
    * @brief Computes the nth term of the Numerical series of pi^2 / 6 - 1
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series() : series_base<T, K>(0, static_cast<T>(std::fma(std::numbers::pi / 6, std::numbers::pi, -1))) {}

template <typename T, typename K>
T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(n ? 1.0 / (n * n * (n + 1)) : 0);
}
