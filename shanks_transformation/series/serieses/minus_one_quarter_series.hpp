#pragma once
#include "series/series_base.hpp"

/**
* @brief Numerical series representation of -1/4 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class minus_one_quarter_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    minus_one_quarter_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of -1/4 * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
minus_one_quarter_series<T, K>::minus_one_quarter_series(T x) : series_base<T, K>(x, static_cast<T>(-0.25) * x)
{
    this->series_name = "-0.25*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T minus_one_quarter_series<T, K>::operator()(K n) const
{
    return static_cast<T>(n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(n * (n + static_cast<K>(2))) : 0); // (22.2) [Rows.pdf]
}
