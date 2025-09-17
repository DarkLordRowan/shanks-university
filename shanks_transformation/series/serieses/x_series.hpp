#pragma once
#include "../series_base.hpp"

/**
* @brief Fourier series of function x, -pi <= x <= pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_series final : public series_base<T, K>
{
public:
    x_series() = delete;

    /**
    * @brief Parameterized constructor to initialize the series with function argument and sum
    * @authors Trudolyubov N.A.
    * @param x The argument for function series
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    */
    x_series(T x);

    /**
    * @brief Computes the nth term of the Fourier series of the sine function
    * @authors Trudolyubov N.A.
    * @param n The number of the term
    * @tparam T The type of the elements in the series, K The type of enumerating integer
    * @return nth term of the Fourier series of the sine functions
    */
    [[nodiscard]] constexpr virtual T operator()(K n) const;

    constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value)
			return abs(x) >= static_cast<T>(PI) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value || std::is_same<T, float_precision>::value)
			return abs(x) >= arbPI || !isfinite(x);
		
		return false;
	}
    
};

template <Accepted T, std::unsigned_integral K>
x_series<T, K>::x_series(T x) : series_base<T, K>(x, x)
{
    this->series_name = "x";
    // Сходится при -π < x < π (ряд Фурье для функции f(x) = x)
    // Расходится при x ≤ -π или x ≥ π

    if (domain_checker(x)) {
        this->throw_domain_error("x must be in (-π, π)");
    }
}

template <Accepted T, std::unsigned_integral K>
constexpr T x_series<T, K>::operator()(K n) const
{
    if(n == 0)
        return static_cast<T>(0);

    const T a_n = this->minus_one_raised_to_power_n(n + 1) * static_cast<T>(2) / static_cast<T>(n);
    return a_n * sin(static_cast<T>(n) * this->x); // (61.4) [Rows.pdf], (1 - 2 * ((n + 1) & 2)) = (-1)^{n+1}
}
