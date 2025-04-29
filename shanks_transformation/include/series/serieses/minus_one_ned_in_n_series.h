
/**
* @brief (-1)^n/n^n series = -0,78343051	p 553
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class minus_one_ned_in_n_series final : public series_base<T, K>
{
public:
    minus_one_ned_in_n_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit minus_one_ned_in_n_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
minus_one_ned_in_n_series<T, K>::minus_one_ned_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.78343051 * x)) {}

template <typename T, typename K>
T minus_one_ned_in_n_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (n == 0)
        return 0;
    return this->x * static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) / std::pow(n, n));
}
