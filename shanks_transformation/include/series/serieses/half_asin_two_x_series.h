
/**
* @brief Maclaurin series of 0.5 * asin(2x) where asin(x) is inverse sine of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class half_asin_two_x_series final : public series_base<T, K>
{
public:
    half_asin_two_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    explicit half_asin_two_x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::asin(2 * x)))
{
    if (std::abs(this->x) > 0.5)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T half_asin_two_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const auto _fact_n = this->fact(n);
    return static_cast<T>(static_cast<T>(this->fact(2 * n)) * std::pow(this->x, 2 * n) / (_fact_n * _fact_n * (2 * n + 1))); // p. 566 typo
}