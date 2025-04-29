#pragma once

#include <cmath>

#include "../series_base.h"

/**
* @brief arcsinhx serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arcsinh_x_series final : public series_base<T, K>
{
public:
    arcsinh_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit arcsinh_x_series(T x);

    /**
    * @brief Computes the nth term of the ln1minx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the ln1minx2 series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
arcsinh_x_series<T, K>::arcsinh_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T arcsinh_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>((std::pow(-1, n) * this->fact(2 * n) * std::pow(this->x, 2 * n + 1)) / (std::pow(this->fact(n), 2) * std::pow(4, n) * (2 * n + 1)));
}
