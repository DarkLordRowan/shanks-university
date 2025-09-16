#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function ln(cos(x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_cosx_series : public series_base<T, K>
{
public:
    ln_cosx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_cosx_series(T x);

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
ln_cosx_series<T, K>::ln_cosx_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(std::cos(x))))
{
    this->series_name = "ln(cos(x))";
    // Сходится при |x| < π/2 (ряд для логарифма косинуса)
    // Расходится при |x| ≥ π/2 (cos(x) ≤ 0, логарифм не определён)

    if (std::abs(x) * 2 >= std::numbers::pi || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < π/2");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_cosx_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 2));
    return static_cast<T>( - std::pow(std::sin(this->x), a)) / static_cast<T>(a); // (50.2) [Rows.pdf]
}
