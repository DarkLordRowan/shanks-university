#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function 2^x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_degree_x_series : public series_base<T, K>
{
public:
    two_degree_x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    two_degree_x_series(T x);

    /**
    * @brief Computes the nth term of the Maclaurin series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Maclaurin series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_degree_x_series<T, K>::two_degree_x_series(T x) : series_base<T, K>(x, static_cast<T>(std::pow(2, x)))
{
    this->series_name = "2^x";
    // Сходится при ∀x ∈ ℝ (экспоненциальная функция 2^x = e^(x·ln2))

    if (!std::isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_degree_x_series<T, K>::operator()(K n) const
{
    return static_cast<T>(std::pow(static_cast<T>(std::log(2)), n) * std::pow(this->x, n) / this->fact(n)); // (92.1) [Rows.pdf]
}
