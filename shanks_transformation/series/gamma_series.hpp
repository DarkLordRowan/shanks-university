#pragma once
#include "series.h"

/**
* @brief Maclaurin series for gamma series
* @authors Pavlova A.R.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class gamma_series : public series_base<T, K>
{
public:
    gamma_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Pavlova A.R.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    gamma_series(T t, T x);

    /**
    * @brief Computes the nth term of the using gamma series
    * @authors Pavlova A.R.
    * @param n The number of the term
    * @param t The parametr for series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the using gamma series
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

private:
    T t;
    T x;
    T a_k(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
gamma_series<T, K>::gamma_series(T t, T x) : series_base<T, K>(x), t(t), x(x)
{
    this->series_name = "Γ(t,x)";
    // Сходится при ∀x ∈ ℝ, t > 0 (неполная гамма-функция)
    // Расходится при t ≤ 0

    if (t <= static_cast <T>(0) || !std::isfinite(x) || !std::isfinite(t)) {
        this->throw_domain_error("t must be > 0 and parameters must be finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
T gamma_series<T, K>::a_k(K n) const
{
    return static_cast<T>(n + 1);
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T gamma_series<T, K>::operator()(K n) const
{
    T a_k_n = a_k(n);
    return static_cast<T>(a_k_n * std::pow(this->t, n) * std::tgamma(a_k_n)); // (102.1) [Rows.pdf]
}

