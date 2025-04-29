
/**
* @brief Numerical series representation of 1
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class one_series final : public series_base<T, K>
{
public:
    /**
    * @brief series constructor
    * @authors Pashkov B.B.
    */
    one_series();

    /**
    * @brief Computes the nth term of the Numerical series of 1
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_series<T, K>::one_series() : series_base<T, K>(0, 1) {}

template <typename T, typename K>
T one_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? static_cast<T>(1.0 / fma(n, n, n)) : 0;
}
