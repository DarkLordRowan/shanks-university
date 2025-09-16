#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function sh(x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class sinh_x2_series : public series_base<T, K>
{
public:
    sinh_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    sinh_x2_series(T x);

    /**
    * @brief Computes the nth term of the sinhx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the sinhx2 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
sinh_x2_series<T, K>::sinh_x2_series(T x) : series_base<T, K>(x, std::sinh(x* x))
{
    this->series_name = "sinh(x²)";
    // Сходится при ∀x ∈ ℝ (ряд для sinh(z) сходится при ∀z ∈ ℝ, где z = x²)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T sinh_x2_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(2, n, 1));
    return static_cast<T>(std::pow(this->x, 2 * a) / this->fact(a)); // (89.2) [Rows.pdf]
}
