
/**
* @brief Maclaurin series of value pi^2/12
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class pi_squared_twelve_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    */
    pi_squared_twelve_series();

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_squared_twelve_series<T, K>::pi_squared_twelve_series() : series_base<T, K>(0, static_cast<T>((std::numbers::pi* std::numbers::pi) / 12)) {}

template <typename T, typename K>
T pi_squared_twelve_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return (this->minus_one_raised_to_power_n(n) / static_cast<T>(((n + 1) * (n + 1))));
}
