
/**
/**
* @brief Maclaurin series of function sqrt(1 + x) - 1 - x/2
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/

#pragma once

#include <cmath>

#include "../series_base.h"


template <typename T, typename K>
class sqrt_1plusx_min_1_min_x_div_2_series final : public series_base<T, K>
{
public:
	sqrt_1plusx_min_1_min_x_div_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	*/
	sqrt_1plusx_min_1_min_x_div_2_series(T x);

	/**
	* @brief Computes the nth term of the Maclaurin series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @return nth term of the Maclaurin series of the sine functions
	*/
	[[nodiscard]] T operator()(K n) const override;
};

template <typename T, typename K>
sqrt_1plusx_min_1_min_x_div_2_series<T, K>::sqrt_1plusx_min_1_min_x_div_2_series(T x) : series_base<T, K>(x, std::sqrt(1 + x) - 1 - x / 2) {}

template <typename T, typename K>
T sqrt_1plusx_min_1_min_x_div_2_series<T, K>::operator()(K n) const
{
	if (n < 0)
		throw std::domain_error("negative integer in the input");

	K temp = 1;
	for (K i = 1; 2 * i <= 2 * n; ++i)
		temp *= 2 * i + 1;
	return static_cast<T>(this->minus_one_raised_to_power_n(n + 3) * std::pow(this->x, n + 2) * temp / (this->fact(n + 2) * std::pow(2, n + 2)));
}
