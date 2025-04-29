
/**
* @brief Numerical series representation of pi/4
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include "../series_base.h"

template <typename T, typename K>
class pi_4_series final : public series_base<T, K>
{
public:
    pi_4_series();

    /**
    * @brief Computes the nth term of the Numerical series of pi/4
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_4_series<T, K>::pi_4_series() : series_base<T, K>(0, static_cast<T>(0.25 * std::numbers::pi)) {}

template <typename T, typename K>
T pi_4_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return series_base<T, K>::minus_one_raised_to_power_n(n) / (2 * n + 1);
}
