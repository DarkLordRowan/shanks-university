#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function Si(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Si_x_series final : public series_base<T, K>
{
public:
    Si_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    Si_x_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Si_x_series<T, K>::Si_x_series(T x) : series_base<T, K>(x)
{
    this->series_name = "Si(x)";
    // Сходится при ∀x ∈ ℝ (интегральный синус, ряд сходится на всей числовой прямой)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Si_x_series<T, K>::operator()(K n) const
{
    const T two_n_1 = static_cast<T>(std::fma(2, n, 1));
    return this->minus_one_raised_to_power_n(n) * std::pow(this->x, two_n_1) / two_n_1 / static_cast<T>( this->fact(std::fma(2,n,1)) ); // (64.1) [Rows.pdf], (1 - 2 * (n & 1)) = (-1)^{n}
}
