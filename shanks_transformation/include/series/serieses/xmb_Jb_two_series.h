
/**
* @brief Maclaurin series of x^(-b) * J_b(2x) where J_b(x) is Bessel function of the first kind of order b
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"
#include "../recurrent_series_base.h"

template <typename T, typename K>
class xmb_Jb_two_series final : public series_base<T, K>
{
public:
    xmb_Jb_two_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series, b The integer constant
    * @param b
    */
    xmb_Jb_two_series(T x, K b);

    /**
    * @brief Computes the nth term of the Maclaurin series of x^(-b) * J_b(2x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] T operator()(K n) const override;
private:

    /**
    * @brief The order of Bessel function
    * @authors Pashkov B.B.
    */
    const K mu;
};

template <typename T, typename K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : series_base<T, K>(x, static_cast<T>(std::pow(x, -b)* std::cyl_bessel_j(b, 2 * x))), mu(b) {}

template <typename T, typename K>
T xmb_Jb_two_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    return static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * std::pow(this->x, 2 * n) / (static_cast<T>(this->fact(n)) * static_cast<T>(this->fact(n + this->mu))));
}
