#pragma once
#include "../series_base.hpp"

/**
* @brief Fourier series of system functions pi*x - x^2, 0 < x < pi
*											x^2 - 3*pi*x + 2*pi^2, pi < x < 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
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

	constexpr inline bool domain_checker(T x) const{ 

		if constexpr ( std::is_floating_point<T>::value)
			return x <= static_cast<T>(0) || x >= static_cast<T>(2.0 * PI) || !isfinite(x);
        
        if constexpr ( std::is_same<T, float_precision>::value)
			return x <= static_cast<T>(0) || x >= static_cast<T>(2.0) * arbPI || !isfinite(x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return x.real() <= static_cast<float_precision>(0) || x.real() >= static_cast<float_precision>(2.0) * arbPI || !isfinite(x);
		
		return false;
	}

	constexpr inline T calculate_sum(T x) const {
		if(domain_checker(x)){ return static_cast<T>(0);}

		if constexpr ( std::is_floating_point<T>::value ){
			return static_cast<T>(0) < x && x < static_cast<T>(PI) ? 
				   static_cast<T>(PI) * x - x * x : 
				   (x * x) - static_cast<T>(3.0 * PI) * x + static_cast<T>(2.0 * PI) * static_cast<T>(PI);
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return static_cast<T>(0) < x && x < arbPI ? 
				   arbPI * x - x * x : 
				   (x * x) - static_cast<T>(3.0) * arbPI * x + static_cast<T>(2.0) * arbPI * arbPI;
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return static_cast<float_precision>(0) < x.real() && x.real() < arbPI ? 
				   static_cast<T>(arbPI) * x - x * x : 
				   (x * x) - static_cast<T>(3.0) * static_cast<T>(arbPI) * x + static_cast<T>(2.0) * static_cast<T>(arbPI * arbPI);
		}
	}

};

template <Accepted T, std::unsigned_integral K>
pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series(T x) : 
series_base<T, K>(
	x, 
	calculate_sum(x)
)
{
	this->series_name = "f(x) = { πx - x², 0 < x < π; x² - 3πx + 2π², π ≤ x < 2π }";
	// Сходится при 0 < x < 2π (ряд Фурье для кусочно-квадратичной функции)
	// Расходится при x ≤ 0 или x ≥ 2π

	if (domain_checker(x)) {
		this->throw_domain_error("x must be in (0, 2π)");
	}
}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_x_minus_x_square_and_x_square_minus_three_pi_x_plus_two_pi_square_series<T, K>::operator()(K n) const
{
	const T a = static_cast<T>(fma(2, n, 1));
	return static_cast<T>(8) / static_cast<T>(std::numbers::pi) * sin(this->x * a) / pow(a, static_cast<T>(3)); // (56.2) [Rows.pdf]
}
