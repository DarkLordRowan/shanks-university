
/**
* @brief integral cos serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class Ci_x_series final : public series_base<T, K>
{
public:
    Ci_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit Ci_x_series(T x);

    /**
    * @brief Computes the nth term of the integral cos series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the integral cos series
    */
    [[nodiscard]] T operator()(K n) const override;

private:
    const T gamma = static_cast<T>(0.57721566490153286060);
};

template <typename T, typename K>
Ci_x_series<T, K>::Ci_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T Ci_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    if (n == 0)
        return gamma + std::log(this->x);

    return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 2 * n) / (2 * n * this->fact(2 * n)));
}
