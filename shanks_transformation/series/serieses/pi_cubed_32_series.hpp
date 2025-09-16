#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (pi^3/32) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_cubed_32_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_cubed_32_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
pi_cubed_32_series<T, K>::pi_cubed_32_series(T x) : series_base<T, K>(x, x * static_cast<T>(pow(std::numbers::pi, 3.0)) / static_cast<T>(32))
{
    this->series_name = "π³x/32";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_cubed_32_series<T, K>::operator()(K n) const
{
    return this->x * this->minus_one_raised_to_power_n(n) / static_cast<T>(pow(fma(2, n + 1, -1), 3)); // (42.2) [Rows.pdf]
}
