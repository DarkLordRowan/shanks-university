#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class two_ln2_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return this->x * log(static_cast<T>(4)); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	two_ln2_series() = delete;


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
	two_ln2_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
two_ln2_series<T, K>::two_ln2_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "2*ln(2)*x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T two_ln2_series<T, K>::calculateTerm(K n) const {
	const T n_temp = static_cast<T>(n + 1);
    const T a = n_temp * n_temp * static_cast<T>(4) - static_cast<T>(1);
    return (static_cast<T>(3) * a + static_cast<T>(2)) / (n_temp * a * a); // (44.2) [Rows.pdf]
}
