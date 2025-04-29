
/**
* @brief Taylor series of Incomplete Gamma function
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class Incomplete_Gamma_func_series final : public series_base<T, K>
{
public:
    Incomplete_Gamma_func_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    Incomplete_Gamma_func_series(T x, T s);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;

private:

    /**
    * @brief The parameter s of the series
    * @authors Trudolyubov N.A.
    */
    const T s;
};

template <typename T, typename K>
Incomplete_Gamma_func_series<T, K>::Incomplete_Gamma_func_series(T x, T s) : series_base<T, K>(x), s(s) {}

template <typename T, typename K>
T Incomplete_Gamma_func_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(std::pow(-1, n) * std::pow(this->x, this->s + n) / (this->fact(n) * (this->s + n)));
}

