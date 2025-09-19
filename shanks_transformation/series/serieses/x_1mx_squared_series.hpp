#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_1mx_squared_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(1) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return abs(config.x) >= static_cast<float_precision>(1) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return this->x / fma(this->x, this->x - static_cast<T>(1), static_cast<T>(1) - this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	x_1mx_squared_series() = delete;


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
	x_1mx_squared_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
x_1mx_squared_series<T, K>::x_1mx_squared_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("|x| must be < 1");
	}

	TermCalculatorBase<T,K>::series_name = "x/(1-x)^2";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T x_1mx_squared_series<T, K>::calculateTerm(K n) const {
	return pow(this->x,  static_cast<T>(n)) * static_cast<T>(n); // (14.4) [Rows.pdf]
}