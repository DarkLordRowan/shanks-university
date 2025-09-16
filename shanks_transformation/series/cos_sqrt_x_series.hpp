#pragma once
#include "series.h"

/**
* @brief Maclaurin series for function cos(sqrt(x))
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_sqrt_x_series : public series_base<T, K>
{
public:
    cos_sqrt_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    cos_sqrt_x_series(T x);

    /**
    * @brief Computes the nth term of the cos(sqrt(x)) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the cos(sqrt(x)) series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
cos_sqrt_x_series<T, K>::cos_sqrt_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::cos(std::sqrt(x))))
{
    this->series_name = "cos(√x)";
    // Сходится при x ≥ 0 (ряд для cos(z) сходится при ∀z ∈ ℝ, где z = √x)
    // Расходится при x < 0 (комплексная область)

    if (x < 0 || !std::isfinite(x)) {
        this->throw_domain_error("x must be ≥ 0");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_sqrt_x_series<T, K>::operator()(K n) const
{
    return series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, n)) / this->fact(2 * n); // (98.2) [Rows.pdf]
}
