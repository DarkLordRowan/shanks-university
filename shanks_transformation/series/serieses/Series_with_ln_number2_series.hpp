#pragma once
#include "series.h"

/**
* @brief Series with ln number2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Series_with_ln_number2_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    Series_with_ln_number2_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
Series_with_ln_number2_series<T, K>::Series_with_ln_number2_series(T x) : series_base<T, K>(x)
{
    this->series_name = "series_with_ln2";
    // Сходится при |x| < 1 (ряд с логарифмическими членами)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Series_with_ln_number2_series<T, K>::operator()(K n) const
{
    const T tmp = std::log(static_cast<T>(n + 2));
    return this->x / std::pow(tmp, tmp); // (77.2) [Rows.pdf]
}
