#pragma once
#include "../series_base.hpp"

/**
* @brief Fourier series of system functions f(x) = -3/4, -3 < x <= 0
*											f(x) = x - 3/4, 0 < x < 3
*											f(x + 6) = f(x)
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_3_div_4_or_x_minus_3_div_4_series final : public series_base<T, K>
{
public:
	minus_3_div_4_or_x_minus_3_div_4_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	minus_3_div_4_or_x_minus_3_div_4_series(T x);

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T operator()(K n) const;

	constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(x) >= static_cast<T>(3) || !isfinite(x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(x) >= static_cast<float_precision>(3) || !isfinite(x); 
		
		return false;
	}

	constexpr inline T calculate_sum(T x) const {
		if(domain_checker(x)){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
			return static_cast<T>(-3) < x && x < static_cast<T>(0) ? 
					static_cast<T>(-0.75) : 
					x - static_cast<T>(-0.75);
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return static_cast<float_precision>(-3) < x.real() && x.real() < static_cast<float_precision>(0) ? 
					static_cast<complex_precision<float_precision>>(-0.75) : 
					x - static_cast<complex_precision<float_precision>>(-0.75);
		}
	}

};

template <Accepted T, std::unsigned_integral K>
minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(T x) : 
series_base<T, K>(
	x, 
	calculate_sum(x)
)
{
	this->series_name = "f(x) = { -3/4, -3 < x < 0; x - 3/4, 0 ≤ x < 3 }";
	// Сходится при -3 < x < 3 (ряд Фурье для кусочно-линейной функции на интервале [-3, 3])
	// Расходится при x ≤ -3 или x ≥ 3

	if (domain_checker(x)) {
		this->throw_domain_error("x must be in (-3, 3)");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::operator()(K n) const
{

	if( n == 0)
		return static_cast<T>(0);

	const T pi_n = static_cast<T>(std::numbers::pi) * static_cast<T>(n);
	const T pi2_n2 = pi_n * pi_n;
	const T sin_pin_2 = static_cast<T>(n       % 2) * this->minus_one_raised_to_power_n(n / 2);
	const T cos_pin_2 = static_cast<T>((n + 1) % 2) * this->minus_one_raised_to_power_n(n / 2);
	const T a_n = sin_pin_2 * static_cast<T>(1.5) / pi_n + static_cast<T>(6) / pi2_n2 * (cos_pin_2 - static_cast<T>(1));
	const T b_n = sin_pin_2 * static_cast<T>(6) / pi2_n2 - static_cast<T>(3) / pi_n * cos_pin_2;
	return a_n * cos(pi_n * this->x / static_cast<T>(6)) + b_n * sin(pi_n * this->x / static_cast<T>(6));  // (59.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}
