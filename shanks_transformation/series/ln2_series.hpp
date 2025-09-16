#pragma once
#include "series.h"

/**
* @brief Numerical series representation of ln(2) * x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln2_series : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln2_series(T x);

    /**
    * @brief Computes the nth term of the Numerical series of ln(2) * x
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln2_series<T, K>::ln2_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(2)) * x)
{
    this->series_name = "ln(2)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln2_series<T, K>::operator()(K n) const
{
    const K a = n + 1;
    return - series_base<T, K>::minus_one_raised_to_power_n(a) * this->x / static_cast<T>(a); // (20.2) [Rows.pdf]
}
