#pragma once
#include "series.h"

/**
* @brief Numerical series representation of pi/3 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_3_series : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_3_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of pi/3 * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_3_series<T, K>::pi_3_series(T x) : series_base<T, K>(x, static_cast<T>(std::numbers::pi) * x / static_cast<T>(3))
{
    this->series_name = "π*x/3";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_3_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 1));
    return this->x / (static_cast<T>(n + 1) * static_cast<T>(a) * static_cast<T>(2 * a - 1)); // (23.2) [Rows.pdf]
}
