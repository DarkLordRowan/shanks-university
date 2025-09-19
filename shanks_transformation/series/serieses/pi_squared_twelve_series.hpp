#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class pi_squared_twelve_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return this->x * static_cast<T>(PI) * static_cast<T>(PI) / static_cast<T>(12); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	pi_squared_twelve_series() = delete;


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
	pi_squared_twelve_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
pi_squared_twelve_series<T, K>::pi_squared_twelve_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "π²x/12";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T pi_squared_twelve_series<T, K>::calculateTerm(K n) const {
	const T n_temp = static_cast<T>(n + 1);
    return this->x * minus_one_raised_to_power_n<T,K>(n) / (n_temp * n_temp); // (41.1) [Rows.pdf]
}
