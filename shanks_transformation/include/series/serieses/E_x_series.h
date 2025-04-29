
/**
* @brief Taylor series of Complete elliptic integral of second kind E
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class E_x_series : public series_base<T, K>
{
public:
    E_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    E_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <typename T, typename K>
E_x_series<T, K>::E_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
constexpr T E_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(std::numbers::pi * this->fact(2 * n) * this->fact(2 * n) * std::pow(this->x, 2 * n) / (2 * (1 - 2 * n) * std::pow(16, n) * std::pow(this->fact(n), 4)));
}
