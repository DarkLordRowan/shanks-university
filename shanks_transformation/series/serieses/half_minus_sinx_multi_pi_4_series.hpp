#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class half_minus_sinx_multi_pi_4_series final : public TermCalculatorBase<T, K>
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
			return config.x < static_cast<T>(0) || config.x > static_cast<T>(PI * 0.5) || !isfinite(config.x); 

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value )
			return config.x.real() < static_cast<float_precision>(0) || config.x.real() > static_cast<float_precision>(0.5) * arbPI || !isfinite(config.x); 
		
		return false;

	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(0.5) - static_cast<T>(0.25) * static_cast<T>(PI) * sin(this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	half_minus_sinx_multi_pi_4_series() : TermCalculatorBase<T, K>() {}


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
	half_minus_sinx_multi_pi_4_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
half_minus_sinx_multi_pi_4_series<T, K>::half_minus_sinx_multi_pi_4_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in [0, π/2]");
	}

	TermCalculatorBase<T,K>::series_name = "0.5-(π/4)*sin(x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T half_minus_sinx_multi_pi_4_series<T, K>::calculateTerm(K n) const {
	const T a = static_cast<T>(fma(2, n, 1));
    return cos(this->x * static_cast<T>(fma(2,n,2))) / (a * (a + static_cast<T>(2))); // (48.2) [Rows.pdf]
}