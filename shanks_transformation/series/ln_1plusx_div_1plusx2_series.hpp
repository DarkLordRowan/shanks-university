#pragma once
#include "series.h"

/**
* @brief Maclaurin series for function ln(1 + x)/(1 + x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1plusx_div_1plusx2_series final : public series_base<T, K>
{
public:
    ln_1plusx_div_1plusx2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln_1plusx_div_1plusx2_series(T x);

    /**
    * @brief Computes the nth term of the ln(1 + x)/(1 + x^2) series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the ln(1 + x)/(1 + x^2) series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln_1plusx_div_1plusx2_series<T, K>::ln_1plusx_div_1plusx2_series(T x) : series_base<T, K>(x, std::log(1 + x) / (1 + x * x))
{
    this->series_name = "ln(1+x)/(1+x²)";
    // Сходится при |x| < 1 (ряд для ln(1+x) сходится при |x| < 1, знаменатель 1+x² > 0 при ∀x ∈ ℝ)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= static_cast <T>(1) || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plusx_div_1plusx2_series<T, K>::operator()(K n) const
{
    const K a = n + 1;
    return static_cast<T>(this->minus_one_raised_to_power_n(n + 2) * std::pow(this->x, a) / static_cast<T>(a * std::pow(1 + (this->x * this->x), a))); // (97.1) [Rows.pdf]
}
