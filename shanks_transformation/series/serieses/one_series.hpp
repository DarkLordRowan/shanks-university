#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of 1 * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of 1 * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
one_series<T, K>::one_series(T x) : series_base<T, K>(x, x)
{
    this->series_name = "1*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T one_series<T, K>::operator()(K n) const
{
    return (n ? this->x / static_cast<T>(fma(n, n, n)) : static_cast<T>(0)); // (21.5) [Rows.pdf]
}
