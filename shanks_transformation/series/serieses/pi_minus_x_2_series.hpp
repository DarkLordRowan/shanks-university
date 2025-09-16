#pragma once
#include "../series_base.hpp"

/**
* @brief Trigonometric series of function (pi - x) / 2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_minus_x_2_series final : public series_base<T, K>
{
public:
    pi_minus_x_2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_minus_x_2_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
pi_minus_x_2_series<T, K>::pi_minus_x_2_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * (static_cast<T>(std::numbers::pi) - x))
{
    this->series_name = "(π-x)/2";
    // Сходится при 0 < x < 2π (ряд Фурье для функции f(x) = x)
    // Расходится при x ≤ 0 или x ≥ 2π

    if (x <= static_cast<T>(0) || x >= static_cast<T>(2 * std::numbers::pi) || !isfinite(x)) {
        this->throw_domain_error("x must be in (0, 2π)");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_minus_x_2_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return sin(a * (this->x)) / a;  // (46.5) [Rows.pdf]
}
