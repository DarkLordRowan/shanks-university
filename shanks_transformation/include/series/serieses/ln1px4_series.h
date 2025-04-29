
/**
* @brief ln1px4 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class ln1px4_series final : public series_base<T, K>
{
public:
    ln1px4_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit ln1px4_series(T x);

    /**
    * @brief Computes the nth term of the ln1px4 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the ln1px4 series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln1px4_series<T, K>::ln1px4_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T ln1px4_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(std::pow(-1, n + 2) * std::pow(this->x, 4 * (n + 1)) / (n + 1));
}
