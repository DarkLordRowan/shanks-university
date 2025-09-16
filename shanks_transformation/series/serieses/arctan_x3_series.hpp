#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function arctan(x^3)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class arctan_x3_series final : public series_base<T, K>
{
public:
    arctan_x3_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    arctan_x3_series(T x);

    /**
    * @brief Computes the nth term of the arctanx3 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the arctanx3 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
arctan_x3_series<T, K>::arctan_x3_series(T x) : series_base<T, K>(x, atan(pow(x, static_cast<T>(3))))
{
    this->series_name = "arctan(x³)";
    // Сходится при |x| ≤ 1 (ряд для arctan(z) сходится при |z| ≤ 1, где z = x³)
    // Расходится при |x| > 1

    if (abs(x) > static_cast<T>(1) || !isfinite(x)) {
        this->throw_domain_error("|x| must be ≤ 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T arctan_x3_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(fma(static_cast <K>(2), n, static_cast <K>(1)));
    return series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x, static_cast<T>(3) * a) / a; // (83.2) [Rows.pdf],
}
