

/**
* @brief Series with ln number1
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class Series_with_ln_number1 final : public series_base<T, K>
{
public:
    Series_with_ln_number1();

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
Series_with_ln_number1<T, K>::Series_with_ln_number1() : series_base<T, K>(0) {}

template <typename T, typename K>
T Series_with_ln_number1<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(std::log(1 + std::pow(n + 1, (n + 1) * (n + 1) + (n + 1) / 2) /
        (std::pow(this->fact(n + 1), n + 1) * std::pow(std::numbers::e, (n + 1) * (n + 1)))));
}

