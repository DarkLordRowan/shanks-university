#pragma once
#include "series/series_base.hpp"

/**
* @brief Numerical series representation of x/2 (version 2)
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class x_two_throught_squares_series final : public series_base<T, K>
{
public:
    x_two_throught_squares_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_two_throught_squares_series(T x);

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
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(T x) : series_base<T, K>(x, x / static_cast<T>(2))
{
    this->series_name = "x/2";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T x_two_throught_squares_series<T, K>::operator()(K n) const
{
    return static_cast<T>(n ? this->x * static_cast<T>(std::fma(2, n * n, -1)) / static_cast<T>(std::fma(4, std::pow(n, 4), 1)) : 0); // (36.2) [Rows.pdf]
}
