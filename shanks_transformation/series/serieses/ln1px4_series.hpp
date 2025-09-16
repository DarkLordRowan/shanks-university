#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function ln(1+x^4)
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln1px4_series final : public series_base<T, K>
{
public:
    ln1px4_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln1px4_series(T x);

    /**
    * @brief Computes the nth term of the ln1px4 series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the ln1px4 series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
ln1px4_series<T, K>::ln1px4_series(T x) : series_base<T, K>(x, log(static_cast<T>(1) + pow(x, static_cast<T>(4))))
{
    this->series_name = "ln(1+x⁴)";
    // Сходится при |x| ≤ 1 (ряд для ln(1+z) сходится при |z| ≤ 1, где z = x⁴)
    // Расходится при |x| > 1

    if (abs(x) >= static_cast<T>(1) || !isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln1px4_series<T, K>::operator()(K n) const
{
    const T a = static_cast<T>(n + 1);
    return series_base<T, K>::minus_one_raised_to_power_n(n + 2) * pow(this->x, static_cast<T>(4) * a) / a; // (81.3) [Rows.pdf]
}
