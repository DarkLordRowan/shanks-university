
/**
* @brief ln(1 + x)/(1 + x^2) serieses
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class ln_1plusx_div_1plusx2 final : public series_base<T, K>
{
public:
    ln_1plusx_div_1plusx2() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    */
    explicit ln_1plusx_div_1plusx2(T x);

    /**
    * @brief Computes the nth term of the ln(1 + x)/(1 + x^2) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @return nth term of the ln(1 + x)/(1 + x^2) series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
ln_1plusx_div_1plusx2<T, K>::ln_1plusx_div_1plusx2(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T ln_1plusx_div_1plusx2<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("The term index must be a non-negative integer");

    return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(this->x, n + 1) / ((n + 1) * std::pow(1 + (this->x * this->x), n + 1)));
}
