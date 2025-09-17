#pragma once
#include "../series_base.hpp"
#include <type_traits>

/**
* @brief Fourier series of system functions sin(x) - 2/pi, 0 <= x <= pi
*											-sin(x) - 2/pi, pi <= x <= 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class abs_sin_x_minus_2_div_pi_series final : public series_base<T, K>
{
public:
	abs_sin_x_minus_2_div_pi_series() = delete;

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	abs_sin_x_minus_2_div_pi_series(T x);

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
			return x < 0 || x > static_cast<T>(2.0 * PI) || !isfinite(x); 

		if constexpr ( std::is_same<T, float_precision>::value)
			return x < static_cast<float_precision>(0) || x > static_cast<float_precision>(2.0) * arbPI || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return x.real() < static_cast<float_precision>(0) || x.real() > static_cast<float_precision>(2.0) * arbPI || !isfinite(x); 
		
		return false;

	}

	constexpr inline T calculate_sum(T x) const {
		if(domain_checker(x)){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value){
			return x < static_cast<T>(PI) ? 
			 sin(x) - static_cast<T>(2.0)/static_cast<T>(PI) :
			-sin(x) - static_cast<T>(2.0)/static_cast<T>(PI);
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return x < arbPI ? 
			 sin(x) - static_cast<T>(2.0)/static_cast<T>(arbPI) :
			-sin(x) - static_cast<T>(2.0)/static_cast<T>(arbPI);
		}

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return x.real() < arbPI ? 
			 sin(x) - static_cast<T>(2.0)/static_cast<T>(arbPI) :
			-sin(x) - static_cast<T>(2.0)/static_cast<T>(arbPI);
		}
	}

};


template <Accepted T, std::unsigned_integral K>
abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(T x) : 
series_base<T, K>(
	x, 
	calculate_sum(x)
)
{
	this->series_name = "|sin(x)| - 2/π";
	// Сходится при 0 ≤ x ≤ 2π (ряд Фурье для функции |sin(x)|)
	// Расходится при x < 0 или x > 2π

	if (domain_checker(x)) {
		this->throw_domain_error("x must be in [0, 2π]");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T abs_sin_x_minus_2_div_pi_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(fma(2, n, 1));
	//a_n = -4 / ( pi * (4 * n * n - 1))
	return static_cast<T>(-4) / static_cast<T>(std::numbers::pi) / static_cast<T>(4 * n * n - 1) * cos(static_cast<T>(2 * n) * this->x); // (57.2) [Rows.pdf]
}