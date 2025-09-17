#pragma once
#include "../series_base.hpp"

/**
* @brief Series of function (1 - sqrt(1 - 4x)) / 2x
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_minus_sqrt_1minus4x_div_2x_series final : public series_base<T, K>
{
public:
	one_minus_sqrt_1minus4x_div_2x_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	one_minus_sqrt_1minus4x_div_2x_series(T x);

	/**
	* @brief Computes the nth term of the series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

	constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) > static_cast<T>(0.25) || x == static_cast<T>(0) || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) > static_cast<float_precision>(0.25) || x == static_cast<T>(0) || !isfinite(x);
		
		return false;
	}

};

template <Accepted T, std::unsigned_integral K>
one_minus_sqrt_1minus4x_div_2x_series<T, K>::one_minus_sqrt_1minus4x_div_2x_series(T x) : 
series_base<T, K>(
	x, 
	(static_cast<T>(1) - sqrt(fma(static_cast<T>(-4), x, static_cast<T>(1)))) / (static_cast<T>(2) * x)
)
{
	this->series_name = "(1-sqrt(1-4x))/(2x)";
	// Сходится при 0 < |x| ≤ 0.25 (производящая функция для чисел Каталана)
	// Расходится при |x| > 0.25 или x = 0

	if (domain_checker(x)) {
		this->throw_domain_error("|x| must be in (0, 0.25]");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T one_minus_sqrt_1minus4x_div_2x_series<T, K>::operator()(K n) const
{
	const T xn = pow(this->x, static_cast<T>(n));
	const T binom_coef = this->binomial_coefficient(static_cast<T>(0.5), n + static_cast<K>(1));
	const T power_2 = this->minus_one_raised_to_power_n(n) * static_cast<T>(pow(2, fma(2, n, 1)));

	return power_2 * binom_coef * xn; // (54.1) [Rows.pdf]
}
