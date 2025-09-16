#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (pi/5 - 1/2) * x
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_six_min_half_series final : public series_base<T, K>
{
public:
    pi_six_min_half_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_six_min_half_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Kreynin R.G.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
pi_six_min_half_series<T, K>::pi_six_min_half_series(T x) : series_base<T, K>(x, x * (static_cast<T>(std::numbers::pi) / static_cast<T>(6) - static_cast<T>(0.5)))
{
    this->series_name = "(π/6 - 0.5)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_six_min_half_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(6, n, 5));
    return this->x * series_base<T, K>::minus_one_raised_to_power_n(n) / (a * (a + static_cast<T>(2))); // (35.2) [Rows.pdf]
}
