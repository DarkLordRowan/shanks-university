#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (5pi/12) * x
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class five_pi_twelve_series final : public series_base<T, K>
{
public:
    five_pi_twelve_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    five_pi_twelve_series(T x);

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
five_pi_twelve_series<T, K>::five_pi_twelve_series(T x) : series_base<T, K>(x, x * static_cast<T>(5) * static_cast<T>(std::numbers::pi) / static_cast<T>(12))
{
    this->series_name = "5πx/12";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T five_pi_twelve_series<T, K>::operator()(K n) const
{
    return this->x * series_base<T, K>::minus_one_raised_to_power_n(static_cast<K>(trunc(n / 3))) / static_cast<T>(fma(2, n, 1)); // (33.2) [Rows.pdf]
}
