
/**
* @brief pi/5 - 1/2 series p 534
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
#pragma once

#include "../series_base.h"

template <typename T, typename K>
class pi_six_min_half_series final : public series_base<T, K>
{
public:
    pi_six_min_half_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit pi_six_min_half_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, static_cast<T>(x* (std::numbers::pi / 6 - 1 / 2))) {}

template <typename T, typename K>
T pi_six_min_half_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / ((6 * n + 5) * (6 * n + 7));
}

