
/**
* @brief Maclaurin series of value pi^3/32
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class pi_cubed_32_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    */
    pi_cubed_32_series();

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_cubed_32_series<T, K>::pi_cubed_32_series() : series_base<T, K>(0, static_cast<T>(std::pow(std::numbers::pi, 3) / 32)) {}

template <typename T, typename K>
T pi_cubed_32_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    T n_temp = static_cast<T>(n + 1);
    return this->minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(2 * n_temp - 1, 3));
}
