#pragma once
#include "series.h"

/**
* @brief Numerical series representation of (pi^2 - 9) * x / 3
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_third_pi_squared_m_nine_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_third_pi_squared_m_nine_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of (pi^2 - 9) * x / 3
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series(T x) : series_base<T, K>(x, static_cast<T>(std::fma(static_cast<T>(std::numbers::pi), static_cast<T>(std::numbers::pi), static_cast<T>(-9))) * x / static_cast<T>(3))
{
    this->series_name = "(π² - 9)*x/3";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_third_pi_squared_m_nine_series<T, K>::operator()(K n) const
{
    const K a = n * (n + static_cast<K>(1));
    return static_cast<T>(n ? this->x / static_cast<T>(a * a) : 0); // (29.2) [Rows.pdf]
}
