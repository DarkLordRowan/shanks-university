#pragma once
#include "../series_base.hpp"

/**
* @brief Maclaurin series of function (pi/8)*cos^2(x) - (1/3)*cos(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
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

template <Accepted T, std::unsigned_integral K>
pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::pi_8_cosx_square_minus_1_div_3_cosx_series(T x) : 
series_base<T, K>(
	x, 
	static_cast<T>(std::numbers::pi * 0.125) * cos(x) * cos(x) - (static_cast<T>(1) / static_cast<T>(3)) * cos(x)
)
{
	this->series_name = "(π/8)*cos²(x)-(1/3)*cos(x)";
	// Сходится при |x| ≤ π/2 (тригонометрический ряд)
	// Расходится при |x| > π/2

	if (abs(x) > static_cast<T>(std::numbers::pi * 0.5) || !isfinite(x)) {
		this->throw_domain_error("|x| must be ≤ π/2");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_8_cosx_square_minus_1_div_3_cosx_series<T, K>::operator()(K n) const
{
	const T temp = static_cast<T>(fma(2, n, 1));
	return this->minus_one_raised_to_power_n(n) * cos(temp + static_cast<T>(2)) * this->x /(temp * (temp + static_cast<T>(2)) * (temp + static_cast<T>(4))); // (52.2) [Rows.pdf]
}
