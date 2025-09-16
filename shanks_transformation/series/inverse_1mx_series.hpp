#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function 1 / (1 - x)
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class inverse_1mx_series final : public series_base<T, K>
{
public:
    inverse_1mx_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    inverse_1mx_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of 1 / (1 - x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
inverse_1mx_series<T, K>::inverse_1mx_series(T x) : series_base<T, K>(x, 1 / (1 - x))
{
    this->series_name = "1/(1-x)";
    // Сходится при |x| < 1 (геометрический ряд)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T inverse_1mx_series<T, K>::operator()(K n) const
{
    return static_cast<T>(std::pow(this->x, n)); // (13.1) [Rows.pdf]
}
