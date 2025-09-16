#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of function 4 * artan(x)
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class four_arctan_series final : public series_base<T, K>
{
public:
    four_arctan_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Bolshakov M.P.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    four_arctan_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the arctan multiplied by four
    * @authors Bolshakov M.P.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
four_arctan_series<T, K>::four_arctan_series(T x) : series_base<T, K>(x, static_cast<T>(4) * static_cast<T>(std::atan(x)))
{
    this->series_name = "4*arctan(x)";
    // Сходится при |x| ≤ 1 (условно при |x| = 1)

    if (std::abs(x) > 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T four_arctan_series<T, K>::operator()(K n) const
{
    const K temp = static_cast<K>(std::fma(2, n, 1));
    return static_cast<T>(4) * series_base<T, K>::minus_one_raised_to_power_n(n) * static_cast<T>(std::pow(this->x, temp)) / static_cast<T>(temp); // (7.1) Rows.pdf
}
