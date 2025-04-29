
/**
* @brief Taylor series of function arctg(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class arctg_x_series final : public series_base<T, K>
{
public:
    arctg_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit arctg_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
arctg_x_series<T, K>::arctg_x_series(T x) : series_base<T, K>(x, std::atan(x)) {}

template <typename T, typename K>
T arctg_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x < -1 or this->x > 1)
        throw std::domain_error("The value x must be between -1 and 1 inclusive");
    return static_cast<T>(std::pow(-1, n) * std::pow(this->x, 2 * n + 1) / (2 * n + 1));
}
