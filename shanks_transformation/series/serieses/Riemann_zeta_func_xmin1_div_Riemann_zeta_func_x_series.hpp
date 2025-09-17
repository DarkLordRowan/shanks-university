#pragma once
#include "../series_base.hpp"

/**
* @brief Dirichlet series of Riemann zeta function of x-1 divided by Riemann zeta function of x (R(x-1) / R(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
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

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return x <= static_cast<T>(2) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return x.real() <= static_cast<float_precision>(2) || !isfinite(x);
		
		return false;
	}
};

template <Accepted T, std::unsigned_integral K>
Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series(T x) : series_base<T, K>(x)
{
    this->series_name = "ζ(x-1)/ζ(x)";
    // Сходится при x > 2 (отношение дзета-функций, ζ(x) сходится при x > 1, ζ(x-1) при x > 2)
    // Расходится при x ≤ 2

    if (domain_checker(x)) {
        this->throw_domain_error("x must be > 2");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T Riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_series<T, K>::operator()(K n) const
{
    if (n == 0)
        return static_cast<T>(0);

    return this->phi(n) / pow(static_cast<T>(n), this->x); // (67.3) [Rows.pdf]
}
