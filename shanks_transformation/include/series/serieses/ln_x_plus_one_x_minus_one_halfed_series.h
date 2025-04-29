
/**
* @brief ln(x+1)/(1-x) / 2 series p557
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class ln_x_plus_one_x_minus_one_halfed_series final : public series_base<T, K>
{
public:
    ln_x_plus_one_x_minus_one_halfed_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    */
    explicit ln_x_plus_one_x_minus_one_halfed_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln_x_plus_one_x_minus_one_halfed_series<T, K>::ln_x_plus_one_x_minus_one_halfed_series(T x) : series_base<T, K>(x, std::log((1 + x) / (1 - x)) / 2)
{
    if (std::abs(x) > 1)
        throw std::domain_error("series diverge");
}

template <typename T, typename K>
T ln_x_plus_one_x_minus_one_halfed_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(std::pow(this->x, 2 * n + 1) / (2 * n + 1));
}
