#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_series final : public TermCalculatorBase<T, K>
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
			return abs(config.x) >= static_cast<T>(PI) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value || std::is_same<T, float_precision>::value)
			return abs(config.x) >= arbPI || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return this->x; }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	x_series() = delete;


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
	x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
x_series<T, K>::x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (-π, π)");
	}

	TermCalculatorBase<T,K>::series_name = "x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T x_series<T, K>::calculateTerm(K n) const {
	if(n == 0)
        return static_cast<T>(0);

    const T a_n = minus_one_raised_to_power_n<T,K>(n + 1) * static_cast<T>(2) / static_cast<T>(n);
    return a_n * sin(static_cast<T>(n) * this->x); // (61.4) [Rows.pdf]
}