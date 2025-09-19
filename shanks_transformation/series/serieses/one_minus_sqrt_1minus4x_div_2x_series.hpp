#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_minus_sqrt_1minus4x_div_2x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) > static_cast<T>(0.25) || config.x == static_cast<T>(0) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) > static_cast<float_precision>(0.25) || config.x == static_cast<T>(0) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return (static_cast<T>(1) - sqrt(fma(static_cast<T>(-4), this->x, static_cast<T>(1)))) / (static_cast<T>(2) * this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	one_minus_sqrt_1minus4x_div_2x_series() = delete;


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
	one_minus_sqrt_1minus4x_div_2x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
one_minus_sqrt_1minus4x_div_2x_series<T, K>::one_minus_sqrt_1minus4x_div_2x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be in (0, 0.25]");
	}

	TermCalculatorBase<T,K>::series_name = "(1-sqrt(1-4x))/(2x)";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T one_minus_sqrt_1minus4x_div_2x_series<T, K>::calculateTerm(K n) const {
	const T xn = pow(this->x, static_cast<T>(n));
	const T binom_coef = binomial_coefficient<T,K>(static_cast<T>(0.5), n + static_cast<K>(1));
	const T power_2 = minus_one_raised_to_power_n<T,K>(n) * static_cast<T>(pow(2, fma(2, n, 1)));

	return power_2 * binom_coef * xn; // (54.1) [Rows.pdf]
}