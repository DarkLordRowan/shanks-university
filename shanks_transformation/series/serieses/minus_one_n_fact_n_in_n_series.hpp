#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class minus_one_n_fact_n_in_n_series final : public TermCalculatorBase<T, K>
{
protected:

    /**
     * @brief 
     * 
     * @param x 
     * @return true 
     * @return false 
     */
    inline bool domain_checker(const SeriesConfig<T,K>& config) const { return config.x != static_cast<T>(0) || !isfinite(config.x); }

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return static_cast<T>(-0.65583160) * this->x; }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	minus_one_n_fact_n_in_n_series() = delete;


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
	minus_one_n_fact_n_in_n_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
minus_one_n_fact_n_in_n_series<T, K>::minus_one_n_fact_n_in_n_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("series converges only at x = 0");
	}

	TermCalculatorBase<T,K>::series_name = "∑(-1)ⁿ·n!/n * x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T minus_one_n_fact_n_in_n_series<T, K>::calculateTerm(K n) const {
	return (n ? this->x * static_cast<T>(fact(n)) * minus_one_raised_to_power_n<T,K>(n) / static_cast<T>(pow(n, n)) : static_cast<T>(0)); // (38.2) [Rows.pdf]
}