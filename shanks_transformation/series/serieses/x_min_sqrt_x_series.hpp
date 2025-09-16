#pragma once
#include "series/series_base.hpp"

/**
* @brief Taylor series of function x - sqrt(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_min_sqrt_x_series final : public series_base<T, K>
{
public:
    x_min_sqrt_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_min_sqrt_x_series(T x);

    /**	return this->x * static_cast<T>(std::sqrt(12) * (1 - 2 * ((n + 1) % 2)) / (std::pow(3, n) * (2 * n + 1))); // (78.1) [Rows.pdf]

    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
x_min_sqrt_x_series<T, K>::x_min_sqrt_x_series(T x) : series_base<T, K>(x, x - std::sqrt(x))
{
    this->series_name = "x-√x";
    // Сходится при 0 < x < 1 (разложение в ряд Тейлора)
    // Расходится при x ≤ 0 или x ≥ 1

    if (x <= static_cast <T>(0) || x >= static_cast <T>(1) || !std::isfinite(x)) {
        this->throw_domain_error("x must be in (0, 1)");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_min_sqrt_x_series<T, K>::operator()(K n) const
{
    if (n == 0 || n == 1)
        return static_cast<T>((1 - this->binomial_coefficient(static_cast<T>(0.5), n)) * std::pow(static_cast<T>(-1) + this->x, n)); // (79.1) [Rows.pdf]
    return static_cast<T>((-this->binomial_coefficient(static_cast<T>(1 / 2), n)) * std::pow(static_cast<T>(-1) + this->x, n)); // (79.1) [Rows.pdf]
}
