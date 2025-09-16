#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (pi^2 / 6 - 1) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_squared_6_minus_one_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_squared_6_minus_one_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of (pi^2 / 6 - 1) * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
pi_squared_6_minus_one_series<T, K>::pi_squared_6_minus_one_series(T x) : 
series_base<T, K>(
    x, 
    x * fma(static_cast<T>(std::numbers::pi) / static_cast<T>(6), static_cast<T>(std::numbers::pi), static_cast<T>(-1))
)
{
    this->series_name = "(π²/6 - 1)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_squared_6_minus_one_series<T, K>::operator()(K n) const
{
    return (n ? this->x / static_cast<T>(n * n * (n + 1)) : static_cast<T>(0)); // (25.3) [Rows.pdf]
}
