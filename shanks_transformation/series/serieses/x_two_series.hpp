#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of x / 2 (version 1)
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_two_series final : public series_base<T, K>
{
public:
    x_two_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Kreynin R.G.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_two_series(T x);

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
x_two_series<T, K>::x_two_series(T x) : series_base<T, K>(x, x * static_cast<T>(0.5))
{
    this->series_name = "x/2";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T x_two_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(fma(2, n, 1));
    return this->x / static_cast<T>(a * (a + 2)); // (34.2) [Rows.pdf]
}
