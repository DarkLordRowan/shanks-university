
/**
* @brief Numerical series representation of ln(2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class ln2_series final : public series_base<T, K>
{
public:
    ln2_series();

    /**
    * @brief Computes the nth term of the Numerical series of ln(2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln2_series<T, K>::ln2_series() : series_base<T, K>(0, static_cast<T>(std::log(2))) {}

template <typename T, typename K>
T ln2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return n ? static_cast<T>(-series_base<T, K>::minus_one_raised_to_power_n(n)) / n : 0;
}
