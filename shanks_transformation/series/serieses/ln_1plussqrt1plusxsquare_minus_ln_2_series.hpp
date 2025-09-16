#pragma once
#include "series/series_base.hpp"

/**
* @brief Maclaurin series of function ln(1 + sqrt(1 + x^2)) - ln(2)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <std::floating_point T, std::unsigned_integral K>
class ln_1plussqrt1plusxsquare_minus_ln_2_series final : public series_base<T, K>
{
public:
	ln_1plussqrt1plusxsquare_minus_ln_2_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	ln_1plussqrt1plusxsquare_minus_ln_2_series(T x);

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
ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::ln_1plussqrt1plusxsquare_minus_ln_2_series(T x) : series_base<T, K>(x, static_cast<T>(std::log(static_cast<T>(0.5) * (static_cast<T>(1) + static_cast<T>(std::hypot(1, x))))))
{
	this->series_name = "ln(1+sqrt(1+x^2))-ln(2)";
	// Сходится при |x| ≤ 1 (биномиальный ряд для sqrt(1+x²))
	// Расходится при |x| > 1

	if (x * x > static_cast <T>(1) || !std::isfinite(x)) {
		this->throw_domain_error("|x| must be ≤ 1");
	}
}

template <std::floating_point T, std::unsigned_integral K>
constexpr T ln_1plussqrt1plusxsquare_minus_ln_2_series<T, K>::operator()(K n) const
{
	const K a = n + 1;
	const K b = static_cast<K>(std::fma(2, n, 2));
	return (this->minus_one_raised_to_power_n(n) * static_cast<T>(this->fact(b - 1)) * static_cast<T>(std::pow(this->x, b))) /
		static_cast<T>(static_cast<K>(pow(static_cast<K>(2), b)) * this->fact(a) * this->fact(a)); // (49.3) [Rows.pdf]
}
