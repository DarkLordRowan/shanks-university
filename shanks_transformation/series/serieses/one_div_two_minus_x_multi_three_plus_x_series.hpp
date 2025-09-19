#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_div_two_minus_x_multi_three_plus_x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(2) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) >= static_cast<float_precision>(2) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(1) / ((static_cast<T>(2) - this->x) * (static_cast<T>(3) + this->x)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	one_div_two_minus_x_multi_three_plus_x_series() = delete;


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
	one_div_two_minus_x_multi_three_plus_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
one_div_two_minus_x_multi_three_plus_x_series<T, K>::one_div_two_minus_x_multi_three_plus_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 2");
	}

	TermCalculatorBase<T,K>::series_name = "1/((2-x)(3+x))";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T one_div_two_minus_x_multi_three_plus_x_series<T, K>::calculateTerm(K n) const {
	return static_cast<T>(0.2) * pow(this->x, static_cast<T>(n)) * (
        static_cast<T>(1) / static_cast<T>(pow(2,n+1)) +
        minus_one_raised_to_power_n<T,K>(n) / static_cast<T>(pow(3,n+1))
    ); //(63.1) [Rows.pdf]
}