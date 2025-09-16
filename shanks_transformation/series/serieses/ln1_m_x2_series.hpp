#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function ln(1 - x^2)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln1_m_x2_series final : public series_base<T, K>
{
public:
    ln1_m_x2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln1_m_x2_series(T x);

    /**
    * @brief Computes the nth term of the ln1minx2 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the ln1minx2 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
ln1_m_x2_series<T, K>::ln1_m_x2_series(T x) : series_base<T, K>(x, std::log(1 - x * x))
{
    this->series_name = "ln(1-x²)";
    // Сходится при |x| < 1 (ряд для ln(1-z) сходится при |z| < 1, где z = x²)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= static_cast <T>(1) || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln1_m_x2_series<T, K>::operator()(K n) const
{
    const K a = n + 1;
    return static_cast<T>(-std::pow(this->x, static_cast <K>(2) * a)) / a; // (85.2) [Rows.pdf]
}
