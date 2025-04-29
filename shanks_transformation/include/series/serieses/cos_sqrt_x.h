
/**
* @brief cos(sqrt(x)) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class cos_sqrt_x final : public series_base<T, K>
{
public:
    cos_sqrt_x() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit cos_sqrt_x(T x);

    /**
    * @brief Computes the nth term of the cos(sqrt(x)) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the cos(sqrt(x)) series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
cos_sqrt_x<T, K>::cos_sqrt_x(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T cos_sqrt_x<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(std::pow(-1, n) * std::pow(this->x, n) / this->fact(2 * n));
}
