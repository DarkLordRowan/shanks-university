#pragma once
#include "series/series_base.hpp"

/**
* @brief Numerical series representation of (pi/4 - ln2/2) * x
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_four_minus_ln2_halfed_series final : public series_base<T, K>
{
public:
    pi_four_minus_ln2_halfed_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_four_minus_ln2_halfed_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_four_minus_ln2_halfed_series<T, K>::pi_four_minus_ln2_halfed_series(T x) : series_base<T, K>(x, x * (static_cast<T>(std::numbers::pi) / static_cast<T>(4) - static_cast<T>(std::log(2)) / static_cast<T>(2)))
{
    this->series_name = "(π/4 - ln(2)/2)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_four_minus_ln2_halfed_series<T, K>::operator()(K n) const
{
    return static_cast<T>(n ? this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(std::trunc(n / 2))) / static_cast<T>(n) : 0); // (32.2) [Rows.pdf]
}
