
/**
/**
* @brief Taylor series of function 1/sqrt(2) * sin(x/sqrt(2))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class one_div_sqrt2_sin_xdivsqrt2_series final : public series_base<T, K>
{
public:
    one_div_sqrt2_sin_xdivsqrt2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit one_div_sqrt2_sin_xdivsqrt2_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x / 2) {}

template <typename T, typename K>
T one_div_sqrt2_sin_xdivsqrt2_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");

#ifdef _WIN32
    return static_cast<T>(std::pow(-1, n / 2) * _jn(static_cast<int>(2 * n + 1), this->x));
#else
    return static_cast<T>(std::pow(-1, n / 2) * jn(static_cast<T>(2 * n + 1), this->x));
#endif
}

