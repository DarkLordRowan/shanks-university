
#pragma once

#include <cmath>

#include "../series_base.h"

/**
* @brief Taylor series of function arcsin(x) - x (The problematic row, possibly due to the double factorial in the denominator, needs to be fixed.)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <typename T, typename K>
class arcsin_x_minus_x_series final : public series_base<T, K>
{
public:
    arcsin_x_minus_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit arcsin_x_minus_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
arcsin_x_minus_x_series<T, K>::arcsin_x_minus_x_series(T x) : series_base<T, K>(x, std::asin(x) - x) {}

template <typename T, typename K>
T arcsin_x_minus_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (std::abs(this->x) > 1)
        throw std::domain_error("Modulus of the value x must be less or equal to 1");
    return static_cast<T>(this->fact(this->fact(2 * n + 1)) * std::pow(this->x, 2 * n + 3) / (this->fact(this->fact(2 * n + 2)) * (2 * n + 3)));
}

