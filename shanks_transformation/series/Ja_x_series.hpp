#pragma once
#include "series.h"

/**
* @brief Taylor series of Bessel function J_b(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Ja_x_series : public series_base<T, K>
{
public:
    Ja_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series, b The integer constant
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    Ja_x_series(T x, T a);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
private:
    /**
    * @brief The order of Bessel function
    * @authors Trudolyubov N.A.
    */
    const T a;
};

template <std::floating_point T, std::unsigned_integral K>
Ja_x_series<T, K>::Ja_x_series(T x, T a) : series_base<T, K>(x), a(a)
{
    this->series_name = "Jₐ(x)";
    // Сходится при ∀x ∈ ℝ (функция Бесселя первого рода)
    // Сходится при ∀a ∈ ℝ

    if (!std::isfinite(x) || !std::isfinite(a)) {
        this->throw_domain_error("x and a must be finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Ja_x_series<T, K>::operator()(K n) const
{
    return this->minus_one_raised_to_power_n(n) * std::pow(this->x / static_cast <T>(2), 2 * n + this->a) / static_cast<T>(this->fact(n) * std::tgamma(n + this->a + 1)); // (95.1) [Rows.pdf]
}
