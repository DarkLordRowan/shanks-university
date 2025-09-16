#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function x^(-b) * J_b(2x) where J_b(x) is Bessel function of the first kind of order b
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class xmb_Jb_two_series final : public series_base<T, K>
{
public:
    xmb_Jb_two_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pashkov B.B.
    * @param x The argument for function series, b The order constant
    * @param b
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    xmb_Jb_two_series(T x, K b);

    /**
    * @brief Computes the nth term of the Maclaurin series of x^(-b) * J_b(2x)
    * @authors Pashkov B.B.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
private:

    /**
    * @brief The order of Bessel function
    * @authors Pashkov B.B.
    */
    const K mu;
};

template <Accepted T, std::unsigned_integral K>
xmb_Jb_two_series<T, K>::xmb_Jb_two_series(T x, K b) : 
series_base<T, K>(
    x,
    static_cast<T>(1) / pow(x, static_cast<T>(b)) //* std::cyl_bessel_j(static_cast<T>(b), static_cast<T>(2) * x)
    ), mu(b)
{
    this->series_name = "x⁻ᵇJᵦ(2x)";
    // Сходится при ∀x ∈ ℝ\{0} (ряд Бесселя сходится на всей числовой прямой)
    // При x = 0 требуется специальное рассмотрение (предел)

    if (!isfinite(x) || x == static_cast<T>(0)) {
        this->throw_domain_error("x must be finite and non-zero");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T xmb_Jb_two_series<T, K>::operator()(K n) const
{
    return series_base<T, K>::minus_one_raised_to_power_n(n) * pow(this->x,static_cast<T>( 2 * n))
        / (static_cast<T>(this->fact(n)) * static_cast<T>(tgamma(n + this->mu + 1))); // (11.3) [Rows.pdf]
}
