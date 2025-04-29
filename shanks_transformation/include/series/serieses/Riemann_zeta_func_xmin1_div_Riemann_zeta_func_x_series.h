
/**
* @brief Dirichlet series of Riemann zeta function of x-1 divided by Riemann zeta function of x (R(x-1) / R(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"

template <typename T, typename K>
class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series final : public series_base<T, K>
{
public:
    Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    */
    explicit Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x);

    /**
    * @brief Computes the nth term of the Dirichlet series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @return nth term of the Dirichlet series of the sine functions
    */
    [[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x) : series_base<T, K>(x) {}

template <typename T, typename K>
T Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::operator()(K n) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    if (this->x <= 2)
        throw std::domain_error("The value x must be greater than 1");
    return static_cast<T>(this->phi(n + 1) / std::pow(n + 1, this->x));
}
