#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of function cos(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class cos_x2_series final : public series_base<T, K>
{
public:
    cos_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    cos_x2_series(T x);

    /**
    * @brief Computes the nth term of the cos(x^2) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the cos(x^2) series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
cos_x2_series<T, K>::cos_x2_series(T x) : series_base<T, K>(x, std::cos(x* x))
{
    this->series_name = "cos(x²)";
    // Сходится при ∀x ∈ ℝ (ряд для cos(z) сходится при ∀z ∈ ℝ, где z = x²)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T cos_x2_series<T, K>::operator()(K n) const
{
    const K a = 2 * n;
    return series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, 2 * a) / this->fact(a)); // (88.2) [Rows.pdf]
}
