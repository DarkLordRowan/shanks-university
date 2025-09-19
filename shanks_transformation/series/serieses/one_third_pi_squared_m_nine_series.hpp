#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class one_third_pi_squared_m_nine_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return fma(static_cast<T>(PI), static_cast<T>(PI), static_cast<T>(-9)) * this->x / static_cast<T>(3); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	one_third_pi_squared_m_nine_series() = delete;


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
	one_third_pi_squared_m_nine_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
one_third_pi_squared_m_nine_series<T, K>::one_third_pi_squared_m_nine_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "(π² - 9)*x/3";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T one_third_pi_squared_m_nine_series<T, K>::calculateTerm(K n) const {
	const T a = static_cast<T>(n * (n + static_cast<K>(1)));
    return (n ? this->x / (a * a) : static_cast<T>(0)); // (29.2) [Rows.pdf]
}
