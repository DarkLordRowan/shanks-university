#pragma once
#include "../series_base.hpp"

/**
* @brief Fourier series of system functions -x - π/4, -pi < x <= 0
*											 -π/4,   0 < x < pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_x_minus_pi_4_or_minus_pi_4_series final : public series_base<T, K>
{
public:
	minus_x_minus_pi_4_or_minus_pi_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_x_minus_pi_4_or_minus_pi_4_series(T x);

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

		if constexpr (std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value)
			return abs(x) >= arbPI || !isfinite(x);
		
		return false;
	}

	constexpr inline T calculate_sum(T x) const {
		if(domain_checker(x)){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value){
			return x<= static_cast<T>(0) && x<static_cast<T>(PI) ? static_cast<T>(-0.25 * PI) :
				   x>  static_cast<T>(0) && x<static_cast<T>(PI) ? static_cast<T>(-0.25 * PI) - x :
				   x;
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return x<= static_cast<float_precision>(0) && x<arbPI ? static_cast<T>(-0.25) * arbPI :
				   x>  static_cast<float_precision>(0) && x<arbPI ? static_cast<T>(-0.25) * arbPI - x :
				   x;
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return x.real()<= static_cast<float_precision>(0) && x.real()<arbPI ? static_cast<T>(-0.25) * static_cast<T>(arbPI) :
				   x.real()>  static_cast<float_precision>(0) && x.real()<arbPI ? static_cast<T>(-0.25) * static_cast<T>(arbPI) - x :
				   x;
		}
	}
};

template <Accepted T, std::unsigned_integral K>
minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(T x)
: series_base<T, K>(
	x,
	calculate_sum(x)
)
{
	this->series_name = "f(x) = { -x - π/4, -π < x < 0; -π/4, 0 ≤ x < π }";
	// Сходится при -π < x < π (ряд Фурье для кусочно-линейной функции)
	// Расходится при x ≤ -π или x ≥ π

	if (domain_checker(x)) {
		this->throw_domain_error("x must be in (-π, π)");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::operator()(K n) const
{
	if (n == 0)
		return static_cast<T>(-std::numbers::pi * 0.5);

	const T a_n = static_cast<T>(2) * static_cast<T>(n % 2) / static_cast<T>(std::numbers::pi) / static_cast<T>(n * n);
	const T b_n = this->minus_one_raised_to_power_n(n) / static_cast<T>(n);
	return a_n * cos(static_cast<T>(n) * this->x) + b_n * sin(static_cast<T>(n) * this->x); // (62.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}
