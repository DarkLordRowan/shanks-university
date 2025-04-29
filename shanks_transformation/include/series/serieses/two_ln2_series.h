
/**
* @brief Maclaurin series of value 2*ln2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class two_ln2_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    */
    two_ln2_series();

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
two_ln2_series<T, K>::two_ln2_series() : series_base<T, K>(0, static_cast<T>(2 * std::log(2))) {}

template <typename T, typename K>
T two_ln2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

    T n_temp = static_cast<T>(n + 1);

    return (12 * n_temp * n_temp - 1) / (n_temp * (4 * n_temp * n_temp - 1) * (4 * n_temp * n_temp - 1));
}