
/**
* @brief Maclaurin series of exp(x^2)*erf(x) where erf(x) is error function of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class exp_squared_erf_series final : public series_base<T, K>
{
public:
    exp_squared_erf_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    */
    exp_squared_erf_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of exp(x^2)*erf(x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
exp_squared_erf_series<T, K>::exp_squared_erf_series(T x) : series_base<T, K>(x, std::exp(x* x)* std::erf(x)) {}

template <typename T, typename K>
T exp_squared_erf_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    const auto result = std::pow(this->x, 2 * n + 1) / std::tgamma(n + 1.5);
    if (!isfinite(result))
        throw std::overflow_error("operator() is too big");
    return static_cast<T>(result);
}
