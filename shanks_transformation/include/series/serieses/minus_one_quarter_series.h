#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

/**
* @brief Numerical series representation of -1/4
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class minus_one_quarter_series final : public series_base<T, K>
{
public:
    minus_one_quarter_series();

    /**
    * @brief Computes the nth term of the Numerical series of -1/4
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
minus_one_quarter_series<T, K>::minus_one_quarter_series() : series_base<T, K>(0, -0.25) {}

template <typename T, typename K>
T minus_one_quarter_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * (n + 2)) : 0;
}
