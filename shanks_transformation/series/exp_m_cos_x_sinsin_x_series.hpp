#pragma once
#include "series.h"

/**
* @brief Maclaurin series of exp(-cos(x)) * sin(sin(x))
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class exp_m_cos_x_sinsin_x_series final : public series_base<T, K>
{
public:
    exp_m_cos_x_sinsin_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    exp_m_cos_x_sinsin_x_series(T x);

    /**
    * @brief Computes the nth term of the exp(-cos(x)) * sin(sin(x)) series
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
exp_m_cos_x_sinsin_x_series<T, K>::exp_m_cos_x_sinsin_x_series(T x) : series_base<T, K>(x, std::exp(-std::cos(x))* std::sin(std::sin(x)))
{
    this->series_name = "exp(-cos(x))*sin(sin(x))";
    // Сходится при ∀x ∈ ℝ (композиция всюду сходящихся функций)
    // Однако для численной стабильности ограничиваем |x| ≤ π

    if (std::abs(x) >= std::numbers::pi || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ π for numerical stability");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T exp_m_cos_x_sinsin_x_series<T, K>::operator()(K n) const
{
    return this->minus_one_raised_to_power_n(n + static_cast<K>(1)) * static_cast<T>(std::sin(static_cast<T>(n) * this->x)) / static_cast<T>(this->fact(n)); // (31.1) [Rows.pdf]
}
