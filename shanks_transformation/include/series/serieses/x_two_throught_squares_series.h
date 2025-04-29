
/**
* @brief x/2 p 552
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class x_two_throught_squares_series final : public series_base<T, K>
{
public:
    x_two_throught_squares_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit x_two_throught_squares_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(T x) : series_base<T, K>(x, x / 2) {}

template <typename T, typename K>
T x_two_throught_squares_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (n == 0)
        return 0;
    return static_cast<T>(this->x * static_cast<T>((2 * (n * n) - 1) / (4 * std::pow(n, 4) + 1)));
}
