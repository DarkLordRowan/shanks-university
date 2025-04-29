
/**
* @brief Numerical series representation of 4 * ln2 - 3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class four_ln2_m_3_series final : public series_base<T, K>
{
public:
    four_ln2_m_3_series();

    /**
    * @brief Computes the nth term of the Numerical series of 4 * ln2 - 3
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
four_ln2_m_3_series<T, K>::four_ln2_m_3_series() : series_base<T, K>(0, static_cast<T>(std::fma(4, std::log(2), -3))) {}

template <typename T, typename K>
T four_ln2_m_3_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? series_base<T, K>::minus_one_raised_to_power_n(n) / (n * n * (n + 1) * (n + 1)) : 0;
}
