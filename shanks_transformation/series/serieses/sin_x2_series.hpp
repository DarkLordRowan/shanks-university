#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function sin(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class sin_x2_series final : public series_base<T, K>
{
public:
    sin_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * tparam T The type of the elements in the series, K The type of enumerating integer
    */
    sin_x2_series(T x);

    /**
    * @brief Computes the nth term of the ln1px4 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the ln1px4 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
sin_x2_series<T, K>::sin_x2_series(T x) : series_base<T, K>(x, sin(x* x))
{
    this->series_name = "sin(x²)";
    // Сходится при ∀x ∈ ℝ (ряд для sin(z) сходится при ∀z ∈ ℝ, где z = x²)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T sin_x2_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(fma(2, n, 1));
    return series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x, static_cast<T>(static_cast<K>(2) * a)) / static_cast<T>(this->fact(a)); // (82.2) [Rows.pdf]
}
