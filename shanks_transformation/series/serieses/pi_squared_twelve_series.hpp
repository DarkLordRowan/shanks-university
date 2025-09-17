#pragma once
#include "../series_base.hpp"

/**
* @brief Numerical series representation of (pi^2/12) * x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_squared_twelve_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    pi_squared_twelve_series(T x);

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
pi_squared_twelve_series<T, K>::pi_squared_twelve_series(T x) : 
series_base<T, K>(
    x, 
    x * static_cast<T>(PI) * static_cast<T>(PI) / static_cast<T>(12)
)
{
    this->series_name = "π²x/12";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_squared_twelve_series<T, K>::operator()(K n) const
{
    const T n_temp = static_cast<T>(n + 1);
    return this->x * this->minus_one_raised_to_power_n(n) / (n_temp * n_temp); // (41.1) [Rows.pdf]
}
