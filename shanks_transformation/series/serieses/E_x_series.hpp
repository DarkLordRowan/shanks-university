#pragma once
#include "series.h"

/**
* @brief Taylor series of Complete elliptic integral of second kind E
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class E_x_series final : public series_base<T, K>
{
public:
    E_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    E_x_series(T x);

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
E_x_series<T, K>::E_x_series(T x) : series_base<T, K>(x)
{
    this->series_name = "E(x)";
    // Сходится при |x| < 1 (полный эллиптический интеграл второго рода)
    // Расходится при |x| ≥ 1

    if (std::abs(x) >= 1 || !std::isfinite(x)) {
        this->throw_domain_error("|x| must be < 1");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T E_x_series<T, K>::operator()(K n) const
{
    if(n == 0)
        return static_cast<T>(std::numbers::pi * 0.5);

    const T coeff = static_cast<T>(this->double_fact(2*n-1)) / static_cast<T>(this->double_fact(2*n));
    return static_cast<T>(-std::numbers::pi * 0.5) * coeff * coeff * std::pow(this->x, static_cast<T>(2*n)) / static_cast<T>(2*n-1); // (72.1) [Rows.pdf]
}
