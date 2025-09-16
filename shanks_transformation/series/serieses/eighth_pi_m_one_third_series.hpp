#pragma once
#include "series/series_base.hpp"

/**
* @brief Numerical series representation of (pi/8 - 1/3) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class eighth_pi_m_one_third_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    eighth_pi_m_one_third_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of (pi/8 - 1/3) * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
eighth_pi_m_one_third_series<T, K>::eighth_pi_m_one_third_series(T x) : series_base<T, K>(x, x * static_cast<T>(std::numbers::pi) / static_cast<T>(8) - static_cast<T>(1) / static_cast<T>(3))
{
    this->series_name = "(π/8 - 1/3)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T eighth_pi_m_one_third_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 1));
    return series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / static_cast<T>(a * (a + static_cast<K>(2)) * (a + static_cast<K>(4))); // (28.2) [Rows.pdf]
}
