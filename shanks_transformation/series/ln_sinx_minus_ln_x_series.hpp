#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function ln(sin(x)) - ln(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_sinx_minus_ln_x_series final : public series_base<T, K>
{
public:
    ln_sinx_minus_ln_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_sinx_minus_ln_x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_sinx_minus_ln_x_series<T, K>::ln_sinx_minus_ln_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(static_cast<T>(std::sin(x)) / x)))
{
    this->series_name = "ln(sin(x)/x)";
    // Сходится при 0 < x ≤ π (ряд для логарифма синуса)
    // Расходится при x ≤ 0 или x > π (sin(x)/x ≤ 0, логарифм не определён)

    if (x <= 0 || x > std::numbers::pi || !std::isfinite(x)) {
        this->throw_domain_error("x must be in (0, π]");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_sinx_minus_ln_x_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(this->x / (static_cast<T>(n + 1) * static_cast<T>(std::numbers::pi)));
    return static_cast<T>(std::log(static_cast<T>(1) - a * a)); // (51.2) [Rows.pdf]
}
