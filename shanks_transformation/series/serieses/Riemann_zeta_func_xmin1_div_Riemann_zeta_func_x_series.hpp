#pragma once
#include "series/series_base.hpp"

/**
* @brief Dirichlet series of Riemann zeta function of x-1 divided by Riemann zeta function of x (R(x-1) / R(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series final : public series_base<T, K>
{
public:
    Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x);

    /**
    * @brief Computes the nth term of the Dirichlet series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Dirichlet series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x) : series_base<T, K>(x)
{
    this->series_name = "ζ(x-1)/ζ(x)";
    // Сходится при x > 2 (отношение дзета-функций, ζ(x) сходится при x > 1, ζ(x-1) при x > 2)
    // Расходится при x ≤ 2

    if (x <= 2 || !std::isfinite(x)) {
        this->throw_domain_error("x must be > 2");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::operator()(K n) const
{
    if (n == 0)
        return static_cast<T>(0);

    const K a = n;
    return this->phi(a) / std::pow(static_cast<T>(a), this->x); // (67.3) [Rows.pdf]
}
