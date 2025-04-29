
/**
* @brief artanhx serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class artanh_x_series final : public series_base<T, K>
{
public:
    artanh_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit artanh_x_series(T x);

    /**
    * @brief Computes the nth term of the artanhx series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the artanhx series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
artanh_x_series<T, K>::artanh_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T artanh_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(std::pow(this->x, 2 * n + 1) / (2 * n + 1));
}

