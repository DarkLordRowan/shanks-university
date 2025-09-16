#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (3 - pi) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class three_minus_pi_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    three_minus_pi_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of (3 - pi) * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
three_minus_pi_series<T, K>::three_minus_pi_series(T x) : series_base<T, K>(x, (static_cast<T>(3) - static_cast<T>(std::numbers::pi)) * x)
{
    this->series_name = "(3-π)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T three_minus_pi_series<T, K>::operator()(K n) const
{
    return (n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this-> x / static_cast<T>(n * (n + 1) * fma(2, n, 1)) : static_cast<T>(0)); // (26.2) [Rows.pdf]
}
