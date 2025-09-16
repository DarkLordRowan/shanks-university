#pragma once
#include "series.h"

/**
* @brief Laurent series of function 1 / ((2 - x)*(3 + x))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_div_two_minus_x_multi_three_plus_x_series final : public series_base<T, K>
{
public:
    one_div_two_minus_x_multi_three_plus_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_div_two_minus_x_multi_three_plus_x_series(T x);

    /**
    * @brief Computes the nth term of the Laurent series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Laurent series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_div_two_minus_x_multi_three_plus_x_series<T, K>::one_div_two_minus_x_multi_three_plus_x_series(T x)
: series_base<T, K>(
    x,
    std::abs(x) < 2 ? 1 / ((2 - x) * (3 + x)) : 0
)
{
    this->series_name = "1/((2-x)(3+x))";
    // Сходится при |x| < 2 (радиус сходимости определяется полюсами в x=2 и x=-3)
    // Расходится при |x| ≥ 2

    if (std::abs(x) >= 2 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 2");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_div_two_minus_x_multi_three_plus_x_series<T, K>::operator()(K n) const
{
    return static_cast<T>(0.2) * std::pow(this->x, static_cast<T>(n)) * (
        static_cast<T>(1) / static_cast<T>(std::pow(2,n+1)) +
        this->minus_one_raised_to_power_n(n) / static_cast<T>(std::pow(3,n+1))
    ); //(63.1) [Rows.pdf]
}
