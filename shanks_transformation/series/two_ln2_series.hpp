#pragma once
#include "series.h"

/**
* @brief Numerical series representation of 2*ln2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_ln2_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    two_ln2_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the cosine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the cosine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_ln2_series<T, K>::two_ln2_series(T x) : series_base<T, K>(x, x * static_cast<T>(static_cast<T>(2) * static_cast<T>(std::log(2))))
{
    this->series_name = "2*ln(2)*x";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_ln2_series<T, K>::operator()(K n) const
{
    const T n_temp = static_cast<T>(n + 1);
    const T a = n_temp * n_temp * static_cast<T>(4) - static_cast<T>(1);
    return (static_cast<T>(3) * a + static_cast<T>(2)) / (n_temp * a * a); // (44.2) [Rows.pdf]
}
