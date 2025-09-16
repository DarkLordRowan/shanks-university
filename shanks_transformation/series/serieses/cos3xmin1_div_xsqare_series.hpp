#pragma once
#include "../series_base.hpp"

/**
* @brief Taylor series of function cos(3x - 1) / x^2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class cos3xmin1_div_xsqare_series final : public series_base<T, K>
{
public:
    cos3xmin1_div_xsqare_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    cos3xmin1_div_xsqare_series(T x);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(T x) : series_base<T, K>(x, (cos(static_cast<T>(3) * x) - static_cast<T>(1)) / (x * x))
{
    this->series_name = "(cos(3x)-1)/x²";
    // Сходится при ∀x ∈ ℝ\{0} (ряд для (cos(z)-1)/z² сходится при ∀z ∈ ℝ\{0}, где z = 3x)
    // При x = 0 требуется специальное рассмотрение (предел)

    if (x == static_cast<T>(0) || !isfinite(x)) {
        this->throw_domain_error("x must be finite and non-zero");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T cos3xmin1_div_xsqare_series<T, K>::operator()(K n) const
{
    const K a = fma(2,n,2);
    return series_base<T, K>::minus_one_raised_to_power_n(n + 1) * static_cast<T>(pow(3, a)) * pow(this->x, static_cast<T>(2*n)) / static_cast<T>(this->fact(a));
}
