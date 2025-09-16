#pragma once
#include "series/series_base.hpp"

/**
* @brief Fourier series of system functions pi*x - x^2, 0 < x < pi
*											x^2 - 3*pi*x + 2*pi^2, pi < x < 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series final : public series_base<T, K>
{
public:
	pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;
};

template <std::floating_point T, std::unsigned_integral K>
pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : series_base<T, K>(x, 0 < x && x < std::numbers::pi ? static_cast<T>(std::numbers::pi) * x - x * x : (x * x) - (static_cast<T>(3) * static_cast<T>(std::numbers::pi) * x) + (2 * static_cast<T>(std::numbers::pi) * static_cast<T>(std::numbers::pi)))
{
	this->series_name = "f(x) = { πx - x², 0 < x < π; x² - 3πx + 2π², π ≤ x < 2π }";
	// Сходится при 0 < x < 2π (ряд Фурье для кусочно-квадратичной функции)
	// Расходится при x ≤ 0 или x ≥ 2π

	if (x <= static_cast<T>(0) || x >= static_cast<T>(2) * static_cast<T>(std::numbers::pi) || !std::isfinite(x)) {
		this->throw_domain_error("x must be in (0, 2π)");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(std::fma(2, n, 1));
	return (static_cast<T>(8) / std::numbers::pi_v<T>) * (static_cast<T>(std::sin(a * (this->x))) / static_cast<T>(std::pow(a, 3))); // (56.2) [Rows.pdf]
}
