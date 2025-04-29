
/**
* @brief 5pi/12 p 528 B1
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"


template <typename T, typename K>
class five_pi_twelve_series final : public series_base<T, K>
{
public:
    five_pi_twelve_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit five_pi_twelve_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, static_cast<T>(x * 5 * std::numbers::pi / 12)) {}

template <typename T, typename K>
T five_pi_twelve_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 3))) / (2 * n + 1);
}
