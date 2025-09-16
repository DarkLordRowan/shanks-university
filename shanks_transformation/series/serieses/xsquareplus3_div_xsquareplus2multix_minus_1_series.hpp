#pragma once
#include "series/series_base.hpp"

/**
* @brief Taylor series of function ((x^2 + 3) / (x^2 + 2*x)) - 1 at x = 1
* @authors Trudolyubov N.A.
* @tparam T - type of the elements in the series, K - type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series final : public series_base<T, K>
{
public:
    xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x - argument for function series
    */
    xsquareplus3_div_xsquareplus2multix_minus_1_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n - number of term
    * @return nth term of Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(T x)
: series_base<T, K>(
    x,
    std::abs(x) < 2 ?
    (x* x + 3) /( x * (x + 2)) - 1
    :
    static_cast<T>(0)
)
{
    this->series_name = "((x²+3)/x)*(x+2)-1";
    // Сходится при |x| < 2 (ряд Лорана, разложение в окрестности бесконечности)
    // Расходится при |x| >= 2

    if (std::abs(x) >= 2 || !std::isfinite(x) || x == 0) {
        this->throw_domain_error("|x| must be > 2 and x ≠ 0");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::operator()(K n) const
{
    if (n == 0)
        return static_cast<T>(1.5) / this->x; //n = -1, но n отрицательным быть не может поэтому сдвигаю на 1

    return this->minus_one_raised_to_power_n(n) * static_cast<T>(7) * std::pow(this->x, static_cast<T>(n - 1)) / static_cast<T>(std::pow(2, n + 1)); // (68.1) [Rows.pdf]
}
