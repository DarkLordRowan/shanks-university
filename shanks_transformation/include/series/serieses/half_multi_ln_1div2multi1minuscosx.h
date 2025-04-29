
/**
* @brief Maclaurin series of function 0.5 * ln(1/(2*(1 - cos(x))))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class half_multi_ln_1div2multi1minuscosx final : public series_base<T, K>
{
public:
    half_multi_ln_1div2multi1minuscosx() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit half_multi_ln_1div2multi1minuscosx(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
half_multi_ln_1div2multi1minuscosx<T, K>::half_multi_ln_1div2multi1minuscosx(T x) : series_base<T, K>(x, static_cast<T>(0.5 * std::log(1 / (2 - 2 * std::cos(x))))) {}

template <typename T, typename K>
T half_multi_ln_1div2multi1minuscosx<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= 0 or this->x >= 2 * std::numbers::pi)
        throw std::domain_error("The x value must be between 0 and 2*pi");
    return std::cos((n + 1) * this->x) / (n + 1);
}
