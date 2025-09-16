#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function 0.5 * asin(2x), where asin(x) is inverse sine of x
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class half_asin_two_x_series : public series_base<T, K>
{
public:
    half_asin_two_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    half_asin_two_x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of 0.5 * asin(2x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
half_asin_two_x_series<T, K>::half_asin_two_x_series(T x) : series_base<T, K>(x, static_cast<T>(0.5) * static_cast<T>(std::asin(static_cast<T>(2) * x)))
{
    this->series_name = "0.5*asin(2x)";
    // Сходится при |x| ≤ 0.5 (абсолютно при |x| < 0.5, условно при |x| = 0.5)

    if (std::abs(x) > static_cast<T>(0.5) || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 0.5");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T half_asin_two_x_series<T, K>::operator()(K n) const
{
    const T _fact_n = static_cast<T>(this->fact(n));
    const K a = 2 * n;
    return static_cast<T>(this->fact(a)) * static_cast<T>(std::pow(this->x, a + 1)) / (_fact_n * _fact_n * static_cast<T>((a + 1))); // (12.1) [Rows.pdf]
}
