#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (4 * ln2 - 3) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class four_ln2_m_3_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    four_ln2_m_3_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of (4 * ln2 - 3) * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
four_ln2_m_3_series<T, K>::four_ln2_m_3_series(T x) : series_base<T, K>(x, x * fma(static_cast<T>(4), log(static_cast<T>(2)), static_cast<T>(-3)))
{
    this->series_name = "(4*ln(2) - 3)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T four_ln2_m_3_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n * (n + static_cast<K>(1)));
    return (n ? series_base<T, K>::minus_one_raised_to_power_n(n) * this->x / (a * a) : static_cast<T>(0)); // (30.2) [Rows.pdf]
}
