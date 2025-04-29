
/**
* @brief cosx2 serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class cos_x2_series final : public series_base<T, K>
{
public:
    cos_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit cos_x2_series(T x);

    /**
    * @brief Computes the nth term of the cosx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the cosx2 series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
cos_x2_series<T, K>::cos_x2_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T cos_x2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 4 * n) / this->fact(2 * n));
}
