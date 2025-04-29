
/**
* @brief Series with ln number2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class Series_with_ln_number2 final : public series_base<T, K>
{
public:
    Series_with_ln_number2();

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
Series_with_ln_number2<T, K>::Series_with_ln_number2() : series_base<T, K>(0) {}

template <typename T, typename K>
T Series_with_ln_number2<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    auto tmp = std::log(n + 2);

    return 1 / static_cast<T>((std::pow(tmp, tmp)));
}

