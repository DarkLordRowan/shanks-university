#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_three_plus_ln3_three_devided_two_plus_two_ln2_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return this->x * (static_cast<T>(-3)  + static_cast<T>(1.5) * log(static_cast<T>(3)) +  log(static_cast<T>(4))); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	minus_three_plus_ln3_three_devided_two_plus_two_ln2_series() = delete;


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
	minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::minus_three_plus_ln3_three_devided_two_plus_two_ln2_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "(-3 + 3/2*ln(3) + 2*ln(2))*x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_three_plus_ln3_three_devided_two_plus_two_ln2_series<T, K>::calculateTerm(K n) const {
	const K n_temp = static_cast<K>(n + 1);
    return this->x / static_cast<T>(n_temp * fma(36, n_temp * n_temp, -1));  // (43.2) [Rows.pdf]
}