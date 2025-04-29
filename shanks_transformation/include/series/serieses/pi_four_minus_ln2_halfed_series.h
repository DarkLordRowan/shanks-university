
/**
* @brief pi/4 - ln2/2 series page 526 part 1 This series = -0.5151, but in reality 0,438825
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class pi_four_minus_ln2_halfed_series final : public series_base<T, K>
{
public:
    pi_four_minus_ln2_halfed_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit pi_four_minus_ln2_halfed_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(x* static_cast<T>((std::numbers::pi / 4 - std::log(2) / 2)))) {}

template <typename T, typename K>
T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (n == 0)
        return 0;
    return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 2))) / n;
}
