#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function sqrt((1 - sqrt(1 - x)) / x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class sqrt_oneminussqrtoneminusx_div_x_series final : public series_base<T, K>
{
public:
    sqrt_oneminussqrtoneminusx_div_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    sqrt_oneminussqrtoneminusx_div_x_series(T x);

    /**
    * @brief Computes the nth term of the series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
sqrt_oneminussqrtoneminusx_div_x_series<T, K>::sqrt_oneminussqrtoneminusx_div_x_series(T x) : 
series_base<T, K>(
    x, 
    sqrt((static_cast<T>(1) - sqrt(static_cast<T>(1) - x)) / x)
)
{
    this->series_name = "sqrt((1-sqrt(1-x))/x)";
    // Сходится при 0 < |x| < 1 (биномиальный ряд для sqrt(1-x))
    // Расходится при |x| ≥ 1 или x = 0

    if (abs(x) >= static_cast<T>(1) || x == static_cast<T>(0) || !isfinite(x)) {
        this->throw_domain_error("|x| must be in (0, 1)");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T sqrt_oneminussqrtoneminusx_div_x_series<T, K>::operator()(K n) const
{
    const K a = 2 * n;
    const K b = 2 * a;
    return static_cast<T>(this->fact(b)) * pow(this->x, static_cast<T>(n)) / 
    (sqrt(static_cast<T>(2)) *  static_cast<T>(pow(2, b) * this->fact(a) * this->fact(a + 1))); // (53.1) [Rows.pdf]
}
