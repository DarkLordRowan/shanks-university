#pragma once
#include "../series_base.hpp"

/**
* @brief  Numerical series (-1)^n n! * x / n^n = -0.65583160
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_one_n_fact_n_in_n_series final : public series_base<T, K>
{
public:
    minus_one_n_fact_n_in_n_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    minus_one_n_fact_n_in_n_series(T x);

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
minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(T x) : series_base<T, K>(x, static_cast<T>(-0.65583160) * x) // -0.65583160 value only for initialization, we can calculate with more accuracy with operator()(K n)
{
    this->series_name = "∑(-1)ⁿ·n!/n * x";
    // Сходится только при x = 0 (факториал растёт быстрее любой геометрической прогрессии)
    // Расходится при ∀x ≠ 0

    if (x != static_cast<T>(0) || !isfinite(x)) {
        this->throw_domain_error("series converges only at x = 0");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_one_n_fact_n_in_n_series<T, K>::operator()(K n) const
{
    return (n ? this->x * static_cast<T>(this->fact(n)) * series_base<T, K>::minus_one_raised_to_power_n(n) / static_cast<T>(pow(n, n)) : static_cast<T>(0)); // (38.2) [Rows.pdf]
}
