
/**
* @brief Numerical series representation of 1/12
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include "../series_base.h"

template <typename T, typename K>
class one_twelfth_series final : public series_base<T, K>
{
public:
    one_twelfth_series();

    /**
    * @brief Computes the nth term of the Numerical series of 1 / 12
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_twelfth_series<T, K>::one_twelfth_series() : series_base<T, K>(0, 1 / 12) {}

template <typename T, typename K>
T one_twelfth_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(1.0 / ((2 * n + 1) * (2 * n + 3) * (2 * n + 5)));
}
