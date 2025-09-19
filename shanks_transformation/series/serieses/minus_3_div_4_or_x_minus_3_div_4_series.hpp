#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_3_div_4_or_x_minus_3_div_4_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { 

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value)
			return abs(config.x) >= static_cast<T>(3) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return abs(config.x) >= static_cast<float_precision>(3) || !isfinite(config.x); 
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

		if constexpr ( std::is_floating_point<T>::value || std::is_same<T, float_precision>::value){
			return this->x < static_cast<T>(0) ? 
					static_cast<T>(-0.75) : 
					this->x - static_cast<T>(-0.75);
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return this->x.real() < static_cast<float_precision>(0) ? 
					static_cast<complex_precision<float_precision>>(-0.75) : 
					this->x - static_cast<complex_precision<float_precision>>(-0.75);
		}
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	minus_3_div_4_or_x_minus_3_div_4_series() = delete;


	/**
	* @brief Computes the nth term of the Maclaurin series of the cosine function
	* @authors Bolshakov M.P.
	* @param n The number of the term
	* @tparam T The type of the elements in the series, K The type of enumerating integer
	* @return nth term of the Maclaurin series of the cosine functions
	*/
	[[nodiscard]] constexpr virtual T calculateTerm(K n) const override;

	/**
	 * @brief 
	 * 
	 * @param config 
	 */
	minus_3_div_4_or_x_minus_3_div_4_series(const SeriesConfig<T,K>& config) ;
};

template <Accepted T, std::unsigned_integral K>
minus_3_div_4_or_x_minus_3_div_4_series<T, K>::minus_3_div_4_or_x_minus_3_div_4_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (-3, 3)");
	}

	TermCalculatorBase<T,K>::series_name = "f(x) = { -3/4, -3 < x < 0; x - 3/4, 0 ≤ x < 3 }";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_3_div_4_or_x_minus_3_div_4_series<T, K>::calculateTerm(K n) const {
	if( n == 0)
		return static_cast<T>(0);

	const T pi_n = static_cast<T>(PI) * static_cast<T>(n);
	const T pi2_n2 = pi_n * pi_n;
	const T sin_pin_2 = static_cast<T>(n       % 2) * minus_one_raised_to_power_n<T,K>(n / 2);
	const T cos_pin_2 = static_cast<T>((n + 1) % 2) * minus_one_raised_to_power_n<T,K>(n / 2);
	const T a_n = sin_pin_2 * static_cast<T>(1.5) / pi_n + static_cast<T>(6) / pi2_n2 * (cos_pin_2 - static_cast<T>(1));
	const T b_n = sin_pin_2 * static_cast<T>(6) / pi2_n2 - static_cast<T>(3) / pi_n * cos_pin_2;
	return a_n * cos(pi_n * this->x / static_cast<T>(6)) + b_n * sin(pi_n * this->x / static_cast<T>(6));  // (59.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}