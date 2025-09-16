#pragma once
#include "series.h"

/**
* @brief Taylor series of Incomplete Gamma function
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class Incomplete_Gamma_func_series final : public series_base<T, K>
{
public:
    Incomplete_Gamma_func_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    Incomplete_Gamma_func_series(T x, T s);

    /**
    * @brief Computes the nth term of the Taylor series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Taylor series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

private:

    /**
    * @brief The parameter s of the series
    * @authors Trudolyubov N.A.
    */
    const T s;
};

template <std::floating_point T, std::unsigned_integral K>
Incomplete_Gamma_func_series<T, K>::Incomplete_Gamma_func_series(T x, T s) : series_base<T, K>(x), s(s)
{
    this->series_name = "γ(s,x)";
    // Сходится при ∀x ∈ ℝ, s > 0 (неполная гамма-функция)
    // Расходится при s ≤ 0

    if (s <= 0 || !std::isfinite(x) || !std::isfinite(s)) {
        this->throw_domain_error("s must be > 0 and parameters must be finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T Incomplete_Gamma_func_series<T, K>::operator()(K n) const
{

    //s(s+1)...(s+n)
    T coeff = s;
    for(K j = 1; j <=n ; ++j)
        coeff *= (s + static_cast<T>(j));

    return std::pow(this->x, static_cast<T>(n) + s) * std::exp(-this->x) / coeff; // (75.1) [Rows.pdf]
}
