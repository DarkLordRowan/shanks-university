#pragma once
#include "../term_calculator.hpp"
#include <type_traits>

/**
* @brief Fourier series of system functions sin(x) - 2/pi, 0 <= x <= pi
*											-sin(x) - 2/pi, pi <= x <= 2*pi
* @authors Trudolyubov N.A.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class abs_sin_x_minus_2_div_pi_series final : public TermCalculatorBase<T, K>
{
protected:

	/**
	 * @brief 
	 * 
	 * @param x 
	 * @return true 
	 * @return false 
	*/
	bool domain_checker(const SeriesConfig<T,K>& config) const { 

		if constexpr ( std::is_floating_point<T>::value)
			return config.x < 0 || config.x > static_cast<T>(2.0 * PI) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, float_precision>::value)
			return config.x < static_cast<float_precision>(0) || config.x > static_cast<float_precision>(2.0) * arbPI || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.x.real() < static_cast<float_precision>(0) || config.x.real() > static_cast<float_precision>(2.0) * arbPI || !isfinite(config.x); 
		
		return false;

	}

	/**
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

		if constexpr ( std::is_floating_point<T>::value){
			return this->x < static_cast<T>(PI) ? 
			 sin(this->x) - static_cast<T>(2.0)/static_cast<T>(PI) :
			-sin(this->x) - static_cast<T>(2.0)/static_cast<T>(PI);
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return this->x < arbPI ? 
			 sin(this->x) - static_cast<T>(2.0)/static_cast<T>(arbPI) :
			-sin(this->x) - static_cast<T>(2.0)/static_cast<T>(arbPI);
		}

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return this->x.real() < arbPI ? 
			 sin(this->x) - static_cast<T>(2.0)/static_cast<T>(arbPI) :
			-sin(this->x) - static_cast<T>(2.0)/static_cast<T>(arbPI);
		}
	}

public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument and sum
	* @authors Trudolyubov N.A.
	* @param x The argument for function series
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	*/
	abs_sin_x_minus_2_div_pi_series() = delete;

	/**
	* @brief Computes the nth term of the Fourier series of the sine function
	* @authors Trudolyubov N.A.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Fourier series of the sine functions
	*/
	[[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

	/**
	 * @brief 
	 * 
	 * @param config 
	 */
	abs_sin_x_minus_2_div_pi_series(const SeriesConfig<T,K>& config);


};

template <Accepted T, std::unsigned_integral K>
abs_sin_x_minus_2_div_pi_series<T, K>::abs_sin_x_minus_2_div_pi_series(const SeriesConfig<T,K>& config){

	if (domain_checker(config)){
		this->throw_domain_error("x must be in [0, 2π]");
	}

	TermCalculatorBase<T,K>::series_name = "|sin(x)| - 2/π";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T abs_sin_x_minus_2_div_pi_series<T, K>::calculateTerm(K n) const {
	const T a = static_cast<T>(fma(2, n, 1));
	return static_cast<T>(-4) / static_cast<T>(PI) / static_cast<T>(4 * n * n - 1) * cos(static_cast<T>(2 * n) * this->x); // (57.2) [Rows.pdf]
}