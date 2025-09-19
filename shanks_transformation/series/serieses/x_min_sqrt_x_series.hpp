#pragma once

#include "../term_calculator.hpp"

/**
* @brief Maclaurin series of hyperbolic cosine
* @authors Pashkov B.B.
* @tparam T The type of the elements in the series, K The type of enumerating integer
*/
template <Accepted T, std::unsigned_integral K>
class x_min_sqrt_x_series final : public TermCalculatorBase<T, K>
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
			return config.x <= static_cast<T>(0) || config.x >= static_cast<T>(1) || !isfinite(config.x);

		if constexpr ( std::is_same<T, complex_precision<float_precision>>::value)
			return config.x.real() <= static_cast<float_precision>(0) || config.x.real() >= static_cast<float_precision>(1) || !isfinite(config.x);
		
		return false;
	}

    /**s
	 * @brief 
	 * 
	 * @param x 
	 * @return constexpr T 
	 */
	T calculate_sum() const  { return this->x - sqrt(this->x); }

public:

	/**
	 * @brief Construct a new cos series object
	 * 
	 */
	x_min_sqrt_x_series() = delete;


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
	x_min_sqrt_x_series(const SeriesConfig<T,K>& config);
};

template <Accepted T, std::unsigned_integral K>
x_min_sqrt_x_series<T, K>::x_min_sqrt_x_series(const SeriesConfig<T,K>& config) {

	if (domain_checker(config)){
		this->throw_domain_error("x must be in (0, 1)");
	}

	TermCalculatorBase<T,K>::series_name = "x-√x";
	TermCalculatorBase<T, K>::x = config.x;
	TermCalculatorBase<T, K>::sum = calculate_sum();

}

template <Accepted T, std::unsigned_integral K>
constexpr T x_min_sqrt_x_series<T, K>::calculateTerm(K n) const {
	if (n == 0 || n == 1)
        return (static_cast<T>(1) - binomial_coefficient<T,K>(static_cast<T>(0.5), n)) * pow(static_cast<T>(-1) + this->x, static_cast<T>(n)); // (79.1) [Rows.pdf]

    return static_cast<T>(-1) * binomial_coefficient<T,K>(static_cast<T>(0.5), n) * pow(static_cast<T>(-1) + this->x, static_cast<T>(n)); // (79.1) [Rows.pdf]
}