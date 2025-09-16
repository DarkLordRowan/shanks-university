#pragma once
#include "../series_base.hpp"

/**
* @brief Trigonometric series of function x/12 * (x^2 - pi^2)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_twelfth_x2_pi2_series final : public series_base<T, K>
{
public:
    x_twelfth_x2_pi2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_twelfth_x2_pi2_series(T x);

    /**
    * @brief Computes the nth term of the Trigonometric series of x/12 * (x^2 - pi^2)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
x_twelfth_x2_pi2_series<T, K>::x_twelfth_x2_pi2_series(T x) : 
series_base<T, K>(
    x, 
    x * fma(x, x, static_cast<T>(-std::numbers::pi) * static_cast<T>(std::numbers::pi)) / static_cast<T>(12)
)
{
    this->series_name = "x/12*(x^2-pi^2)";
    // Сходится при |x| ≤ π (ряд Фурье для функции f(x) = x³)
    // Расходится при |x| > π

    if (abs(x) > static_cast<T>(std::numbers::pi) || !isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ π");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T x_twelfth_x2_pi2_series<T, K>::operator()(K n) const
{
    const K a = n + 1;
    return series_base<T, K>::minus_one_raised_to_power_n(a) * sin(static_cast<T>(a) * this->x) / static_cast<T>(pow(a, 3)); // (19.2) [Rows.pdf]
}
