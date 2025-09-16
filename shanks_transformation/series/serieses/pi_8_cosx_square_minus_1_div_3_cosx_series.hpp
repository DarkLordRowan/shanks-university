#pragma once
#include "series.h"

/**
* @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class pi_8_cosx_square_minus_1_div_3_cosx_series final : public series_base<T, K>
{
public:
	pi_8_cosx_square_minus_1_div_3_cosx_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	pi_8_cosx_square_minus_1_div_3_cosx_series(T x);

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
pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::pi_8_cosx_square_minus_1_div_3_cosx_series(T x) : series_base<T, K>(x, (static_cast<T>(std::numbers::pi) / static_cast<T>(8)) * static_cast<T>(std::cos(x)) * static_cast<T>(std::cos(x)) - ((static_cast<T>(1) / static_cast<T>(3)) * static_cast<T>(std::cos(x))))
{
	this->series_name = "(π/8)*cos²(x)-(1/3)*cos(x)";
	// Сходится при |x| ≤ π/2 (тригонометрический ряд)
	// Расходится при |x| > π/2

	if (std::abs(x) > std::numbers::pi / 2 || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be ≤ π/2");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::operator()(K n) const
{
	const K temp = static_cast<K>(std::fma(2, n, 1));
	return this->minus_one_raised_to_power_n(n) * static_cast<T>(std::cos(temp + static_cast<T>(2))) * static_cast<T>(this->x) /
		static_cast<T>((temp * (temp + 2) * (temp + 4))); // (52.2) [Rows.pdf]
}
