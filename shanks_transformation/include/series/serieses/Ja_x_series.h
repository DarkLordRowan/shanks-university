
/**
* @brief Taylor series of Bessel function J_b(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class Ja_x_series final : public series_base<T, K>
{
public:
    Ja_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series, b The integer constant
    * @param a
    */
    Ja_x_series(T x, T a);

    /**
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
private:
    const T a;
};

template <typename T, typename K>
Ja_x_series<T, K>::Ja_x_series(T x, T a) : series_base<T, K>(x), a(a) {}

template <typename T, typename K>
T Ja_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return this->minus_one_raised_to_power_n(n) * std::pow(this->x / 2, 2 * n + this->a) / (this->fact(n) * std::tgamma(n + this->a + 1));
}

