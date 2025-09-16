#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function arsinh(x)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class arcsinh_x_series final : public series_base<T, K>
{
public:
    arcsinh_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arcsinh_x_series(T x);

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
arcsinh_x_series<T, K>::arcsinh_x_series(T x) : series_base<T, K>(x, std::asinh(x))
{
    this->series_name = "arcsinh(x)";
    // Сходится при |x| ≤ 1 (ряд для гиперболического арксинуса)
    // Расходится при |x| > 1

    if (std::abs(x) > static_cast <T>(1) || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T arcsinh_x_series<T, K>::operator()(K n) const
{
    const K a = static_cast<K>(std::fma(static_cast <K>(2), n, static_cast <K>(1)));
    return static_cast<T>(series_base<T, K>::minus_one_raised_to_power_n(n) * this->double_fact(a) * std::pow(this->x, a + static_cast <K>(2))) / static_cast<T>((this->double_fact(a + static_cast <K>(1)) * (a + static_cast <K>(2))));  // (87.2) [Rows.pdf]
}
