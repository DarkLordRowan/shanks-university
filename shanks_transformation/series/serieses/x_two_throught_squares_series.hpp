#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_two_throught_squares_series final : public TermCalculatorBase<T, K>
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
	T calculate_sum() const  { return this->x * static_cast<T>(0.5); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	x_two_throught_squares_series() = delete;


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
	x_two_throught_squares_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
x_two_throught_squares_series<T, K>::x_two_throught_squares_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x is not finite");
	}

	TermCalculatorBase<T,K>::series_name = "x/2";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T x_two_throught_squares_series<T, K>::calculateTerm(K n) const {
	return (n ? 
        this->x * static_cast<T>(fma(2, n * n, -1)) / static_cast<T>(fma(4, pow(n, 4), 1)) : 
        static_cast<T>(0)
    ); // (36.2) [Rows.pdf]
}
