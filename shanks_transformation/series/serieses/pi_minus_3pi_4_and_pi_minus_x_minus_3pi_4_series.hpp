#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series final : public TermCalculatorBase<T, K>
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
			return config.x <= -static_cast<T>(PI) || config.x > static_cast<T>(PI) || !isfinite(config.x);
        
        if constexpr ( std::is_same<T, float_precision>::value)
			return config.x <= -arbPI || config.x > arbPI || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return config.x.real() <= -arbPI || config.x.real() > arbPI || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  {

		if constexpr ( std::is_floating_point<T>::value ){
			return this->x < static_cast<T>(0) ? 
				   static_cast<T>(0.25 * PI): 
				   static_cast<T>(0.25 * PI) - this->x;
		}

		if constexpr ( std::is_same<T, float_precision>::value){
			return this->x < static_cast<T>(0) ? 
				   static_cast<T>(0.25) * arbPI: 
				   static_cast<T>(0.25) * arbPI - this->x;
		}


		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value ){
			return this->x.real() < static_cast<float_precision>(0) ? 
                   static_cast<T>(static_cast<float_precision>(0.25) * arbPI): 
				   static_cast<T>(static_cast<float_precision>(0.25) * arbPI) - this->x;
		}
	}

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series() = delete;


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
	pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (-π, π]");
	}

	TermCalculatorBase<T,K>::series_name = "f(x) = { pi/4, -π < x < 0; π/4 - x , 0 ≤ x ≤ π }";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_series<T, K>::calculateTerm(K n) const {
	if(n == 0)
		return static_cast<T>(0);

	const T a_n = static_cast<T>((n % 2) * 2) / (static_cast<T>(std::numbers::pi) * static_cast<T>(n * n));
	const T b_n = minus_one_raised_to_power_n<T,K>(n) / static_cast<T>(n);
	return a_n * cos(static_cast<T>(n) * this->x) + b_n * sin(static_cast<T>(n) * this->x); // (58.5) [Rows.pdf]
}