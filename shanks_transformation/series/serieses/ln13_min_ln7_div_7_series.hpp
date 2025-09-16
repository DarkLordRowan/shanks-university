#pragma once
#include "../series_base.hpp"

/**
* @brief Numetric series of function (ln13 - ln7) * x / 7
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln13_min_ln7_div_7_series final : public series_base<T, K>
{
public:
    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    ln13_min_ln7_div_7_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <Accepted T, std::unsigned_integral K>
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series(T x) : series_base<T, K>(x, x * log(static_cast<T>(13) / static_cast<T>(7)) / static_cast<T>(7))
{
    this->series_name = "(ln(13 / 7))·x/7";
    // Сходится при ∀x ∈ ℝ (линейная функция)

    if (!isfinite(x)) {
        this->throw_domain_error("x is not finite");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T ln13_min_ln7_div_7_series<T, K>::operator()(K n) const
{
    const K a = n + 1;
    const K b = n + 2;
    return this->minus_one_raised_to_power_n(b) * this->x * static_cast<T>(pow(6, a)) / static_cast<T>(a * pow(7, b)); // (94.2) [Rows.pdf]
}
