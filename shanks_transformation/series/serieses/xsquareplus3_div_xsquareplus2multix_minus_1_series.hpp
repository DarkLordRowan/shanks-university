#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class xsquareplus3_div_xsquareplus2multix_minus_1_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) < static_cast<T>(2) || !isfinite(config.x) || config.x != static_cast<T>(0); 

		if constexpr ( std::is_same<T, float_precision>::value || std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) < static_cast<float_precision>(2) || !isfinite(config.x) || config.x != static_cast<T>(0);
		
		return false;

	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return (this->x * this->x + static_cast<T>(3)) /( this->x * (this->x + static_cast<T>(2))) - static_cast<T>(1); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	xsquareplus3_div_xsquareplus2multix_minus_1_series() = delete;


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
	xsquareplus3_div_xsquareplus2multix_minus_1_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::xsquareplus3_div_xsquareplus2multix_minus_1_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be > 2 and x ≠ 0");
	}

	TermCalculatorBase<T,K>::series_name = "((x²+3)/x)*(x+2)-1";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T xsquareplus3_div_xsquareplus2multix_minus_1_series<T, K>::calculateTerm(K n) const {
	if (n == 0)
        return static_cast<T>(1.5) / this->x; //n = -1, но n отрицательным быть не может поэтому сдвигаю на 1

    return minus_one_raised_to_power_n<T,K>(n) * static_cast<T>(7) * pow(this->x, static_cast<T>(n - 1)) / static_cast<T>(pow(2, n + 1)); // (68.1) [Rows.pdf]
}