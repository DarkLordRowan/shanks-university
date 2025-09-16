#pragma once
#include "series/series_base.hpp"

/**
* @brief Taylor series of function 1/sqrt(2) * sin(x/sqrt(2))
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class one_div_sqrt2_sin_xdivsqrt2_series final : public series_base<T, K>
{
public:
    one_div_sqrt2_sin_xdivsqrt2_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    one_div_sqrt2_sin_xdivsqrt2_series(T x);

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
one_div_sqrt2_sin_xdivsqrt2_series<T, K>::one_div_sqrt2_sin_xdivsqrt2_series(T x) : series_base<T, K>(x, static_cast<T>((1 / std::sqrt(2))* std::sin(x / std::sqrt(2))))
{
    this->series_name = "(1/√2)*sin(x/√2)";
    // Сходится при ∀x ∈ ℝ (ряд для sin(z) сходится при ∀z ∈ ℝ, где z = x/√2)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T one_div_sqrt2_sin_xdivsqrt2_series<T, K>::operator()(K n) const
{
#ifdef _WIN32
    return static_cast<T>(std::pow(-1, n / 2) * _jn(static_cast<int>(2 * n + 1), this->x)); // (96.1) [Rows.pdf]
#else
    return static_cast<T>(std::pow(-1, n / 2) * jn(static_cast<T>(2 * n + 1), this->x));
#endif
}
