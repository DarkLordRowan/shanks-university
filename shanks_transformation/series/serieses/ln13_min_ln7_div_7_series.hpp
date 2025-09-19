#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class ln13_min_ln7_div_7_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { return !isfinite(config.x); }

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return this->x * log(static_cast<T>(13) / static_cast<T>(7)) / static_cast<T>(7); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	ln13_min_ln7_div_7_series() = delete;


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
	ln13_min_ln7_div_7_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
ln13_min_ln7_div_7_series<T, K>::ln13_min_ln7_div_7_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "(ln(13 / 7))·x/7";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T ln13_min_ln7_div_7_series<T, K>::calculateTerm(K n) const {
	const K a = n + 1;
    const K b = n + 2;
    return minus_one_raised_to_power_n<T,K>(b) * this->x * static_cast<T>(pow(6, a)) / static_cast<T>(a * pow(7, b)); // (94.2) [Rows.pdf]
}
