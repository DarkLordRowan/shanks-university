
/**
* @brief Taylor series of Lambert W function
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"


template <typename T, typename K>
class Lambert_W_func_series final : public series_base<T, K>
{
public:
    Lambert_W_func_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit Lambert_W_func_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
Lambert_W_func_series<T, K>::Lambert_W_func_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T Lambert_W_func_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (std::abs(this->x) >= 1 / std::numbers::e)
        throw std::domain_error("The absolute value of x must be less 1/e");
    return static_cast<T>(std::pow(-n - 1, n) * std::pow(this->x, n + 1) / this->fact(n + 1));
}

