#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function 2 arcsin(x/2)^2
* @authors Kreynin R.G.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class two_arcsin_square_x_halfed_series final : public series_base<T, K>
{
public:
	two_arcsin_square_x_halfed_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Kreynin R.G.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	two_arcsin_square_x_halfed_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Kreynin R.G.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
two_arcsin_square_x_halfed_series<T, K>::two_arcsin_square_x_halfed_series(T x) : series_base<T, K>(x, static_cast<T>(2) * static_cast<T>(std::asin(x / static_cast<T>(2))) * static_cast<T>(std::asin(x / static_cast<T>(2))))
{
	this->series_name = "2*arcsin(x/2)^2";
	// Сходится при |x| ≤ 2 (ряд для arcsin(z) сходится при |z| ≤ 1, где z = x/2)
	// Расходится при |x| > 2

	if (std::abs(x) > 2 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be ≤ 2");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T two_arcsin_square_x_halfed_series<T, K>::operator()(K n) const
{
	const K a = static_cast<K>(std::fma(2, n, 1));
	// return (this->binomial_coefficient(2 * n, n) * this->binomial_coefficient(2 * n, n) * std::pow(this->x, 2 * a)) / (a * a * std::pow(2, std::fma(3, a, -2)));
	return static_cast<T>((std::pow(this->x, a - 1)) *  static_cast<T>(this->fact(n)) * static_cast<T>(this->fact(n))) / (static_cast<T>(2 * (n + 1)) * static_cast<T>(this->fact(a)));
}
