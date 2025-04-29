

/**
* @brief Maclaurin series of exp(-cos(x)) * sin(sin(x))
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class exp_m_cos_x_sinsin_x_series final : public series_base<T, K>
{
public:
    exp_m_cos_x_sinsin_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit exp_m_cos_x_sinsin_x_series(T x);

    /**
    * @brief Computes the nth term of the exp(-cos(x)) * sin(sin(x)) series
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(T x) : series_base<T, K>(x, std::exp(-std::cos(x))* std::sin(std::sin(x))) {}

template <typename T, typename K>
T exp_m_cos_x_sinsin_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return this->minus_one_raised_to_power_n(n) * std::sin(n * this->x) / this->fact(n);
}
