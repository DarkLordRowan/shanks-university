#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_x_minus_pi_4_or_minus_pi_4_series final : public TermCalculatorBase<T, K>
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

		if constexpr ( std::is_floating_point<T>::value)
			return abs(config.x) >= static_cast<T>(PI) || !isfinite(config.x); 

		if constexpr (std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) >= arbPI || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

		if constexpr ( std::is_floating_point<T>::value){
			return this->x<static_cast<T>(PI) ? 
			static_cast<T>(-0.25 * PI) :
			static_cast<T>(-0.25 * PI) - this->x;
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return this->x<arbPI ? 
			static_cast<T>(-0.25) * arbPI :
			static_cast<T>(-0.25) * arbPI - this->x;
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return this->x.real()<arbPI ? 
			static_cast<T>(-0.25) * static_cast<T>(arbPI) :
			static_cast<T>(-0.25) * static_cast<T>(arbPI) - this->x;
		}
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	minus_x_minus_pi_4_or_minus_pi_4_series() = delete;


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
	minus_x_minus_pi_4_or_minus_pi_4_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::minus_x_minus_pi_4_or_minus_pi_4_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (-π, π)");
	}

	TermCalculatorBase<T,K>::series_name = "f(x) = { -x - π/4, -π < x < 0; -π/4, 0 ≤ x < π }";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_x_minus_pi_4_or_minus_pi_4_series<T, K>::calculateTerm(K n) const {
	if (n == 0)
		return static_cast<T>(-std::numbers::pi * 0.5);

	const T a_n = static_cast<T>(2) * static_cast<T>(n % 2) / static_cast<T>(PI) / static_cast<T>(n * n);
	const T b_n = minus_one_raised_to_power_n<T,K>(n) / static_cast<T>(n);
	return a_n * cos(static_cast<T>(n) * this->x) + b_n * sin(static_cast<T>(n) * this->x); // (62.5) [Rows.pdf], (1 - 2 * ((n + 1) & 1)) = (-1)^{n+1}
}