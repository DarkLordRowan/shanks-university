#pragma once
#include "series/series_base.hpp"

/**
* @brief Taylor series of number pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_series(T x);

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
pi_series<T, K>::pi_series(T x) : series_base<T, K>(x)
{
    this->series_name = "π";
    // Сходится при ∀x ∈ ℝ (константа π)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_series<T, K>::operator()(K n) const
{
    return this->minus_one_raised_to_power_n(n) * this->x * std::sqrt(static_cast<T>(12)) / static_cast<T>(fma(2,n,1)) / static_cast<T>(std::pow(3,n)); // (78.1) [Rows.pdf], (n + 1) % 2 is the same (n + 1) & 1
}
