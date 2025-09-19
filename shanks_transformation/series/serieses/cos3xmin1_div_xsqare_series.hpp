#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of cosine function
* @authors Bolshakov M.P.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class cos3xmin1_div_xsqare_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const{ return config.x == static_cast<T>(0) || !isfinite(config.x); }

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return (cos(static_cast<T>(3) * this->x) - static_cast<T>(1)) / (this->x * this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	cos3xmin1_div_xsqare_series() = delete;


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
	cos3xmin1_div_xsqare_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
cos3xmin1_div_xsqare_series<T, K>::cos3xmin1_div_xsqare_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be finite and non-zero");
	}

	TermCalculatorBase<T,K>::series_name = "(cos(3x)-1)/x²";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T cos3xmin1_div_xsqare_series<T, K>::calculateTerm(K n) const {
	const K a = fma(2,n,2);
    return minus_one_raised_to_power_n<T,K>(n + 1) * static_cast<T>(pow(3, a)) * pow(this->x, static_cast<T>(2*n)) / static_cast<T>(fact<K>(a));
}